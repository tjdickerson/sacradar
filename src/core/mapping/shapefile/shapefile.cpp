//
// Created by tjdic on 6/9/2024.
//

#include "shapefile.h"

#include <cassert>


bool read_index_file(BufferInfo *buffer, const char *filename) {
    std::string index_filename = filename;
    index_filename.append(".").append(SF_EXT_INDEX);

    FILE *fp = fopen(index_filename.c_str(), "rb");
    if (!fp) {
        LOGERR("Error opening file %s\n", index_filename.c_str());
        return false;
    }

    fseek(fp, 0, SEEK_END);
    const size_t file_length = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    const auto data = (unsigned char *)malloc(file_length * sizeof(unsigned char));
    assert(data != nullptr);

    const size_t bytes_read = fread(data, 1, file_length, fp);
    assert(bytes_read == file_length);

    init_buffer(buffer, data, bytes_read);

    fclose(fp);
    return true;
}

bool read_shape_file(ShapeFileInfo *shapefile_info, const char *filepath) {
    BufferInfo index_buffer = {};
    if (!read_index_file(&index_buffer, filepath) || index_buffer.length == 0) {
        LOGERR("Failed to index file for %s.\n", filepath);
        free_buffer(&index_buffer);
        return false;
    }

    ShapeFileHeader  index_header       = {};
    ShapeIndexRecord index_record       = {};
    ShapeIndexRecord shape_index_record = {};

    read_from_buffer(&index_header, &index_buffer, sizeof(s32) * 9);
    read_from_buffer(&index_header.mbr, &index_buffer, sizeof(f64) * 8);

    index_header.file_length = swap_bytes(index_header.file_length);
    index_header.file_code   = swap_bytes((u32)index_header.file_code); // NOLINT(*-narrowing-conversions)
    assert(index_header.file_code == 9994);

    // Save buffer position and read through file to get length and feature count.
    const s32 buffer_pos     = index_buffer.position;
    u32       content_length = 0;
    u32       feature_count  = 0;

    while (index_buffer.position < index_buffer.length) {
        read_from_buffer(&index_record, &index_buffer, sizeof(index_record));
        content_length += swap_bytes(index_record.content_length);
        feature_count += 1;
    }

    // Reset buffer position to previous mark.
    set_buffer_pos(&index_buffer, buffer_pos);

    std::string shapefile_path = filepath;
    shapefile_path.append(".").append(SF_EXT_SHAPE);

    FILE *fp = fopen(shapefile_path.c_str(), "rb");
    if (fp == nullptr) {
        LOGERR("Faile to open shapefile: %s.\n", shapefile_path.c_str());
        free_buffer(&index_buffer);
        return false;
    }

    fseek(fp, 0, SEEK_END);
    size_t file_length = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    BufferInfo  t_shape_buffer = {};
    BufferInfo *shape_buffer   = &t_shape_buffer;

    const auto shape_file_data = (unsigned char *)malloc(file_length * sizeof(unsigned char));
    fread(shape_file_data, sizeof(unsigned char), file_length, fp);
    init_buffer(shape_buffer, shape_file_data, file_length);

    shapefile_info->num_features     = feature_count;
    shapefile_info->features         = (ShapeFeature *)malloc(feature_count * sizeof(ShapeFeature));
    shapefile_info->total_num_points = 0;
    shapefile_info->total_num_parts  = 0;

    // @todo
    // For the moment just allocate plenty of memory for these as they are shrunken later
    shapefile_info->points = (f32 *)malloc(file_length * sizeof(unsigned char));
    shapefile_info->starts = (s32 *)malloc(file_length * sizeof(unsigned char));
    shapefile_info->counts = (s32 *)malloc(file_length * sizeof(unsigned char));

    s32 points_offset        = 0;
    s32 point_accum          = 0;
    s32 parts_offset         = 0;
    s32 actual_feature_count = 0;

    switch (index_header.shape_type) {
    case SHAPE_TYPE_POLYLINE:
        shapefile_info->shape_type = PolyLine;
        break;
    case SHAPE_TYPE_POLYGON:
        shapefile_info->shape_type = Polygon;
        break;
    default:
        shapefile_info->shape_type = Undefined;
    }

    for (int i = 0; i < feature_count; i++) {
        read_from_buffer(&index_record, &index_buffer, sizeof(index_record));
        index_record.offset = swap_bytes(index_record.offset) * 2;

        set_buffer_pos(shape_buffer, index_record.offset);
        ShapeFeature *feature = &shapefile_info->features[actual_feature_count];
        feature->parts_index  = parts_offset;
        actual_feature_count += 1;

        read_from_buffer(&shape_index_record, shape_buffer, sizeof(index_record));

        s32 record_shape_type;
        read_from_buffer(&record_shape_type, shape_buffer, sizeof(record_shape_type));
        assert(record_shape_type == index_header.shape_type);

        f64 bounding_box[4];
        read_from_buffer(bounding_box, shape_buffer, sizeof(f64) * 4);
        feature->bounding_box       = {};
        feature->bounding_box.min_x = (f32)bounding_box[0];
        feature->bounding_box.min_y = (f32)bounding_box[1];
        feature->bounding_box.max_x = (f32)bounding_box[2];
        feature->bounding_box.max_y = (f32)bounding_box[3];

        s32 num_parts;
        read_from_buffer(&num_parts, shape_buffer, sizeof(s32));
        feature->num_parts = num_parts;
        shapefile_info->total_num_parts += num_parts;

        s32 num_points;
        read_from_buffer(&num_points, shape_buffer, sizeof(s32));
        feature->num_points = num_points;
        shapefile_info->total_num_points += num_points;

        for (int j = 0; j < num_parts; j++) {
            s32 this_start;
            read_from_buffer(&this_start, shape_buffer, sizeof(s32));

            this_start += points_offset;
            shapefile_info->starts[parts_offset] = this_start;
            parts_offset += 1;
        }

        for (int j = 0; j < num_parts; j++) {
            const s32 index       = (parts_offset - num_parts) + j;
            s32       count_value = 0;

            if (j < num_parts - 1) {
                count_value = shapefile_info->starts[index + 1];
            }
            else {
                count_value = num_points - (shapefile_info->starts[index] - points_offset);
            }

            shapefile_info->counts[index] = count_value;
        }

        for (int j = 0; j < num_points; j++) {
            v2f64 point;
            read_from_buffer(&point, shape_buffer, sizeof(point));

            const s32 point_index                   = point_accum;
            shapefile_info->points[point_index]     = (f32)point.x;
            shapefile_info->points[point_index + 1] = (f32)point.y;
            point_accum += 2;
        }

        points_offset += num_points;
    }

    shapefile_info->num_features = actual_feature_count;

    const auto new_starts  = (s32 *)realloc(shapefile_info->starts, shapefile_info->total_num_parts * sizeof(s32));
    shapefile_info->starts = new_starts;

    const auto new_counts  = (s32 *)realloc(shapefile_info->counts, shapefile_info->total_num_parts * sizeof(s32));
    shapefile_info->counts = new_counts;

    free_buffer(shape_buffer);

    return true;
}
