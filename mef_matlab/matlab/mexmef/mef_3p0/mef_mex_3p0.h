//
//  mef_mex.h
//  mef_3p0

//  Copyright (c) Richard J. Cui Created: Wed 05/29/2019  9:49:29.694 PM
//  $Revision: 0.1 $  $Date: Wed 05/29/2019  9:49:29.694 PM $
//
//  Rocky Creek Dr NE
//  Rochester, MN 55906, USA
//
//  Email: richard.cui@utoronto.ca
//
//  (Including code adapted from M.V.D. Boom - vandenboom.max@mayo.edu)

#ifndef mef_mex_h
#define mef_mex_h

#include "mex.h"
#include "meflib.h"

//  defines
#define RANGE_BY_SAMPLES     0
#define RANGE_BY_TIME        1

//  MATLAB Structures
// Universal Header
const int UNIVERSAL_HEADER_NUMFIELDS        = 21;
const char *UNIVERSAL_HEADER_FIELDNAMES[]   = {
    "header_CRC",
    "body_CRC",
    "file_type_string",
    "mef_version_major",
    "mef_version_minor",
    "byte_order_code",
    "start_time",
    "end_time",
    "number_of_entries",
    "maximum_entry_size",
    "segment_number",
    "channel_name",                    // utf8[63], base name only, no extension
    "session_name",                    // utf8[63], base name only, no extension
    "anonymized_name",                // utf8[63]
    "level_UUID",
    "file_UUID",
    "provenance_UUID",
    "level_1_password_validation_field",
    "level_2_password_validation_field",
    "protected_region",
    "discretionary_region"
};

// Segment
const int SEGMENT_NUMFIELDS         = 11;
const char *SEGMENT_FIELDNAMES[]    = {
    "channel_type",
    //"metadata_fps",                // instead of mapping the FILE_PROCESSING_STRUCT object, the data held within is mapped to the 'metadata' field
    //"time_series_data_fps",        // instead of mapping the FILE_PROCESSING_STRUCT object, the data held within is mapped to the 'time_series_data_uh' field
    //"time_series_indices_fps",    // instead of mapping the FILE_PROCESSING_STRUCT object, the indices held within are mapped to the 'time_series_indices' field
    //"video_indices_fps",            // instead of mapping the FILE_PROCESSING_STRUCT object, the indices held within are mapped to the 'video_indices' field
    //"record_data_fps",            // instead of mapping the FILE_PROCESSING_STRUCT object, the records-data held within is mapped to the 'records' field
    //"record_indices_fps",            // instead of mapping the FILE_PROCESSING_STRUCT object, the records-indices held within are mapped to the 'records' field
    "name",                            // just base name, no extension
    "path",                            // full path to enclosing directory (channel directory)
    "channel_name",                    // just base name, no extension
    "session_name",                    // just base name, no extension
    "level_UUID",
    
    // non-meflib fields, added for mapping (contains data normally held within the _fps structs)
    "metadata",
    "time_series_indices",
    "video_indices",
    "records",
    "time_series_data_uh"
};

//  Channel
const int CHANNEL_NUMFIELDS         = 15;
const char *CHANNEL_FIELDNAMES[]    = {
    "channel_type",
    "metadata",
    //"record_data_fps",            // instead of mapping the FILE_PROCESSING_STRUCT object, the records-data held within is mapped to the 'records' field
    //"record_indices_fps",            // instead of mapping the FILE_PROCESSING_STRUCT object, the records-indices held within are mapped to the 'records' field
    "number_of_segments",
    "segments",
    "path",                            // full path to enclosing directory (session directory)
    "name",                            // just base name, no extension
    "extension",                    // channel directory extension
    "session_name",                    // just base name, no extension
    "level_UUID",
    "anonymized_name",
    // variables below refer to segments
    "maximum_number_of_records",
    "maximum_record_bytes",
    "earliest_start_time",
    "latest_end_time",
    
    // non-meflib fields, added for mapping (contains data normally held within the _fps structs)
    "records"
};

//  Session
const int SESSION_NUMFIELDS         = 15;
const char *SESSION_FIELDNAMES[]    = {
    "time_series_metadata",
    "number_of_time_series_channels",
    "time_series_channels",
    "video_metadata",
    "number_of_video_channels",
    "video_channels",
    //"record_data_fps",            // instead of mapping the FILE_PROCESSING_STRUCT object, the record-data held within is mapped to the 'records' field
    //"record_indices_fps",            // instead of mapping the FILE_PROCESSING_STRUCT object, the record-indices held within are mapped to the 'records' field
    "name",
    "path",
    "anonymized_name",
    "level_UUID",
    // below variables refer to channels
    "maximum_number_of_records",
    "maximum_record_bytes",
    "earliest_start_time",
    "latest_end_time",

    // non-meflib fields, added for mapping (contains data normally held within the _fps structs)
    "records"
};

// Metadata Structures
const int METADATA_SECTION_1_NUMFIELDS      = 4;
const char *METADATA_SECTION_1_FIELDNAMES[] = {
    "section_2_encryption",
    "section_3_encryption",
    "protected_region",                // (not mapped)
    "discretionary_region"            // (not mapped)
};

//  Time series metadata section 2
const int TIME_SERIES_METADATA_SECTION_2_NUMFIELDS      = 27;
const char *TIME_SERIES_METADATA_SECTION_2_FIELDNAMES[] = {
    // type-independent fields
    "channel_description",            // utf8[511];
    "session_description",            // utf8[511];
    "recording_duration",
    // type-specific fields
    "reference_description",        // utf8[511];
    "acquisition_channel_number",
    "sampling_frequency",
    "low_frequency_filter_setting",
    "high_frequency_filter_setting",
    "notch_filter_frequency_setting",
    "AC_line_frequency",
    "units_conversion_factor",
    "units_description",            // utf8[31]
    "maximum_native_sample_value",
    "minimum_native_sample_value",
    "start_sample",
    "number_of_samples",
    "number_of_blocks",
    "maximum_block_bytes",
    "maximum_block_samples",
    "maximum_difference_bytes",
    "block_interval",
    "number_of_discontinuities",
    "maximum_contiguous_blocks",
    "maximum_contiguous_block_bytes",
    "maximum_contiguous_samples",
    "protected_region",                // (not mapped)
    "discretionary_region"            // (not mapped)
};

//  Video metada section 2
const int VIDEO_METADATA_SECTION_2_NUMFIELDS        = 12;
const char *VIDEO_METADATA_SECTION_2_FIELDNAMES[]   = {
    // type-independent fields
    "channel_description",            // utf8[511]
    "session_description",
    "recording_duration",
    // type-specific fields
    "horizontal_resolution",
    "vertical_resolution",
    "frame_rate",
    "number_of_clips",
    "maximum_clip_bytes",
    "video_format",
    "video_file_CRC",
    "protected_region",                // (not mapped)
    "discretionary_region"            // (not mapped)
};

//  Metadata section 3
const int METADATA_SECTION_3_NUMFIELDS      = 10;
const char *METADATA_SECTION_3_FIELDNAMES[] = {
    "recording_time_offset",
    "DST_start_time",
    "DST_end_time",
    "GMT_offset",
    "subject_name_1",                // utf8[31]
    "subject_name_2",                // utf8[31]
    "subject_ID",                    // utf8[31]
    "recording_location",            // utf8[127]
    "protected_region",                // (not mapped)
    "discretionary_region"            // (not mapped)
};

//  Metadata
const int METADATA_NUMFIELDS        = 3;
const char *METADATA_FIELDNAMES[]   = {
    "section_1",
    "section_2",                    // non-meflib field, added for mapping 'time_series_section_2' or 'video_section_2' (since it is always one or the other)
    //"time_series_section_2",        // instead of mapping the time_series_section_2 object, the data is mapped to the 'section_2' field for time-series
    //"video_section_2",            // instead of mapping the video_section_2 object, the data is mapped to the 'section_2' field for video
    "section_3"
};

//  Record
const int RECORD_HEADER_NUMFIELDS       = 7;
const char *RECORD_HEADER_FIELDNAMES[]  = {
    "record_CRC",
    "type_string",
    "version_major",
    "version_minor",
    "encryption",
    "bytes",
    "time"
};

//  Record index
const int RECORD_INDEX_NUMFIELDS        = 6;
const char *RECORD_INDEX_FIELDNAMES[]   = {
    "type_string",
    "version_major",
    "version_minor",
    "encryption",
    "file_offset",
    "time"
};


// Time series Indices
const int TIME_SERIES_INDEX_NUMFIELDS       = 11;
const char *TIME_SERIES_INDEX_FIELDNAMES[]  = {
    "file_offset",
    "start_time",
    "start_sample",
    "number_of_samples",
    "block_bytes",
    "maximum_sample_value",
    "minimum_sample_value",
    "protected_region",                    // (not mapped)
    "RED_block_flags",
    "RED_block_protected_region",        // (not mapped)
    "RED_block_discretionary_region"    // (not mapped)
};

// Video Indices
const int VIDEO_INDEX_NUMFIELDS         = 8;
const char *VIDEO_INDEX_FIELDNAMES[]    = {
    "start_time",
    "end_time",
    "start_frame",
    "end_frame",
    "file_offset",
    "clip_bytes",
    "protected_region",                    // (not mapped)
    "discretionary_region"                // (not mapped)
};


// File Processing
// TODO: check whether File Processing is used
//  see FILE_PROCESSING_STRUCT in meflib.h
const int FILE_PROCESSINGFILE_PROCESSING_NUMFIELDS      = 16;
const char *FILE_PROCESSING_FIELDNAMES[]                = {
    "full_file_name",                    // full path including extension
    "fp",                                // runtime file pointer        (not mapped)
    "fd",                                // runtime file descriptor    (not mapped)
    "file_length",
    "file_type_code",
    "universal_header",
    "directives",                        // runtime struct     (not mapped)
    "password_data",                    // this will often be the same for all files
    "metadata",
    "time_series_indices",
    "video_indices",
    "records",
    "record_indices",
    "RED_blocks",                        // only used when type = TIME_SERIES_DATA_FILE_TYPE_CODE; (not mapped)
    "raw_data_bytes",
    "raw_data"                            // (not mapped)
};


// Record Type
const int MEFREC_CSTI_1_0_NUMFIELDS         = 4;
const char *MEFREC_CSTI_1_0_FIELDNAMES[]    = {
    "task_type",
    "stimulus_duration",
    "stimulus_type",
    "patient_response"
};

//  functions
mxArray *read_channel_data_from_path(si1*, si1*, bool, si8, si8);
mxArray *read_channel_data_from_object(CHANNEL*, bool, si8, si8);
si8 sample_for_uutc_c(si8, CHANNEL*);
si8 uutc_for_sample_c(si8, CHANNEL*);
void memset_int(si4*, si4, size_t);
si4 check_block_crc(ui1*, ui4, ui1*, ui8);

void map_mef3_segment_tostruct(SEGMENT*, si1, mxArray*, int);
mxArray *map_mef3_segment(SEGMENT*, si1 );
void map_mef3_channel_tostruct(CHANNEL*, si1, mxArray*, int);
mxArray *map_mef3_channel(CHANNEL*, si1);
mxArray *map_mef3_session(SESSION*, si1);
mxArray *map_mef3_md1(METADATA_SECTION_1*);
mxArray *map_mef3_tmd2(TIME_SERIES_METADATA_SECTION_2*);
mxArray *map_mef3_vmd2(VIDEO_METADATA_SECTION_2*);
mxArray *map_mef3_md3(METADATA_SECTION_3*);
mxArray *map_mef3_ti(TIME_SERIES_INDEX*, si8);
mxArray *map_mef3_vi(VIDEO_INDEX*, si8);
mxArray *map_mef3_records(FILE_PROCESSING_STRUCT*, FILE_PROCESSING_STRUCT*);
mxArray *map_mef3_csti(RECORD_HEADER*);
mxArray *map_mef3_uh(UNIVERSAL_HEADER*);

mxArray *mxUint8ArrayByValue(ui1*, int);
mxArray *mxUint8ByValue(ui1);
mxArray *mxInt8ByValue(si1);
mxArray *mxUint32ByValue(ui4);
mxArray *mxInt32ByValue(si4);
mxArray *mxUint64ByValue(ui8);
mxArray *mxInt64ByValue(si8);
mxArray *mxDoubleByValue(sf8);
mxArray *mxStringByUTF8Value(char*);

#endif /* mef_mex_h */

// [EOF]
