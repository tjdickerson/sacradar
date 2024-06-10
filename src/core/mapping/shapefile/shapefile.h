//
// Created by tjdic on 6/9/2024.
//

#ifndef SHAPEFILE_H
#define SHAPEFILE_H
#include <string>


#include "tjd_share.h"


inline auto SF_EXT_INDEX = "shx";
inline auto SF_EXT_SHAPE = "shp";
inline auto SF_EXT_DBASE = "dbf";

#define SHAPE_STATES 0x01
#define SHAPE_COUNTIES 0x02
#define SHAPE_ROADS 0x03
#define SHAPE_RIVERS 0x04

#define SHAPE_TYPE_POLYLINE 3
#define SHAPE_TYPE_POLYGON 5


struct ShapeFile2dBr {
    f32 min_x;
    f32 min_y;
    f32 max_x;
    f32 max_y;
};

typedef struct ShapeFile_MBR_t {
    f64 min_x;
    f64 min_y;
    f64 max_x;
    f64 max_y;
    f64 min_z;
    f64 max_z;
    f64 min_m;
    f64 max_m;
} ShapeFile_MBR;

typedef struct ShapeFileHeader_t {
    s32           file_code;
    u32           unused1;
    u32           unused2;
    u32           unused3;
    u32           unused4;
    u32           unused5;
    u32           file_length;
    s32           version;
    s32           shape_type;
    ShapeFile_MBR mbr;
} ShapeFileHeader;

typedef struct ShapeIndexRecord_t {
    u32 offset;
    u32 content_length;
} ShapeIndexRecord;

typedef struct ShapeFileRecHeader_t {
    s32 record_number;
    s32 record_length;
} ShapeFileRecHeader;

struct TextMarker {
    std::string text{};
    u32         textLength{};
    v2f32       location{};
};

struct ShapeFeature {
    s32 parts_index{};
    s32 num_parts{};
    s32 num_points{};

    ShapeFile2dBr bounding_box{};
    TextMarker    feature_name{};
};

enum ShapeType { Undefined = 0, PolyLine = 3, Polygon = 5 };

struct ShapeFileInfo {
    u32           num_features{};
    u32           total_num_points;
    u32           total_num_parts;
    ShapeType     shape_type;
    ShapeFeature *features{};
    std::string   filename{};

    s32* starts{};
    s32* counts{};
    f32* points{};
};

/**
 * Reads ESRI shapefile into the ShapeFeature array of the passed in ShapeFileInfo.
 * @param shapefile_info
 * @param filepath This should point to a shapefile without extension. Assumes files with same name will exist with
 * required extensions (.shp, .shx, optionial .dbf)
 * @return
 */
bool read_shape_file(ShapeFileInfo *shapefile_info, const char *filepath);


#endif // SHAPEFILE_H
