/**
*     @file
*     MEF 3.0 Library Matlab Wrapper
*     Read a MEF3 folder and retrieve the session, channel(s), segment(s) and record(s) metadata
*
*  Copyright 2020, Max van den Boom (Multimodal Neuroimaging Lab, Mayo Clinic, Rochester MN)
*    Adapted from PyMef (by Jan Cimbalnik, Matt Stead, Ben Brinkmann, and Dan Crepeau)
*
*
*  This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License
*  as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
*  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
*  warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*  You should have received a copy of the GNU General Public License along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

//  Modified by Richard J. Cui: Wed 05/29/2019  9:49:29.694 PM
//  $Revision: 0.5 $  $Date: Mon 11/02/2020  9:21:42.834 AM $
//
//  Rocky Creek Dr NE
//  Rochester, MN 55906, USA
//
//  Email: richard.cui@utoronto.ca

#include "mex.h"
#include "mef_mex_3p0.h"
#include "meflib.c"
#include "mefrec.c"

///
// Functions for converting c-objects to matlab-structs
///

/**
 *     Map a MEF segment c-struct to an exising matlab-struct
 *
 *    The given matlab-struct have multiple entries, the MEF
 *    struct will be mapped on the given index
 *
 *     @param segment            Pointer to the MEF segment c-struct
 *     @param map_indices_flag
 *     @param mat_segment        Pointer to the existing matlab-struct
 *     @param mat_index        The index in the existing matlab-struct at which to map the data
 */
void map_mef3_segment_tostruct(SEGMENT *segment, si1 map_indices_flag, mxArray *mat_segment, int mat_index) {

    // set segment properties
    mxSetField(mat_segment, mat_index, "channel_type",                 mxInt32ByValue(segment->channel_type));
    mxSetField(mat_segment, mat_index, "name",                         mxCreateString(segment->name));
    mxSetField(mat_segment, mat_index, "path",                         mxCreateString(segment->path));
    mxSetField(mat_segment, mat_index, "channel_name",                 mxCreateString(segment->channel_name));
    mxSetField(mat_segment, mat_index, "session_name",                 mxCreateString(segment->session_name));
    mxSetField(mat_segment, mat_index, "level_UUID",                 mxUint8ArrayByValue(segment->level_UUID, UUID_BYTES));
    
    
    //
    // file processing struct
    //
    if (segment->channel_type == TIME_SERIES_CHANNEL_TYPE)
        mxSetField(mat_segment, mat_index, "time_series_data_uh",         map_mef3_uh(segment->time_series_data_fps->universal_header));

    
    //
    // records
    //
    
    // check segment records and (if existent) add
    if ((segment->record_indices_fps != NULL) & (segment->record_data_fps != NULL))
        mxSetField(mat_segment, mat_index, "records", map_mef3_records(segment->record_indices_fps, segment->record_data_fps));
    
    
    //
    // metadata
    //
    
    // create a metadata struct (for the segment) and assign it to the 'metadata' field
    mxArray *segment_metadata_struct = mxCreateStructMatrix(1, 1, METADATA_NUMFIELDS, METADATA_FIELDNAMES);
    mxSetField(mat_segment, mat_index, "metadata", segment_metadata_struct);
    
    // map segment metadata sections
    mxSetField(segment_metadata_struct, 0, "section_1", map_mef3_md1(segment->metadata_fps->metadata.section_1));
    switch (segment->channel_type){
        case TIME_SERIES_CHANNEL_TYPE:
            mxSetField(segment_metadata_struct, 0, "section_2", map_mef3_tmd2(segment->metadata_fps->metadata.time_series_section_2));
            break;
        case VIDEO_CHANNEL_TYPE:
            mxSetField(segment_metadata_struct, 0, "section_2", map_mef3_vmd2(segment->metadata_fps->metadata.video_section_2));
            break;
        default:
            mexErrMsgTxt("Unrecognized channel type, exiting...");
    }
    mxSetField(segment_metadata_struct, 0, "section_3", map_mef3_md3(segment->metadata_fps->metadata.section_3));
    
    
    //
    // map indices
    //
        
    // check if indices should be mapped
    if (map_indices_flag) {
        
        switch (segment->channel_type){
            case TIME_SERIES_CHANNEL_TYPE:
        
                // create a time-series indices struct (for the segment) and assign it to the 'time_series_indices' field
                mxSetField(    mat_segment,
                            mat_index,
                            "time_series_indices",
                            map_mef3_ti(    segment->time_series_indices_fps->time_series_indices,
                                            segment->time_series_indices_fps->universal_header->number_of_entries));
                
                break;
            case VIDEO_CHANNEL_TYPE:
                
                // create a video indices struct (for the segment) and assign it to the 'video_indices' field
                mxSetField(    mat_segment,
                            mat_index,
                            "video_indices",
                            map_mef3_vi(    segment->video_indices_fps->video_indices,
                                            segment->video_indices_fps->universal_header->number_of_entries));
                
                break;
            default:
                mexErrMsgTxt("Unrecognized channel type, exiting...");
                
        }
    }
    
}
 
/**
 *     Map a MEF segment c-struct to a newly created matlab-struct
 *
 *     @param segment            Pointer to the MEF segment c-struct
 *     @param map_indices_flag
 *     @return                    Pointer to the new matlab-struct
 */
mxArray *map_mef3_segment(SEGMENT *segment, si1 map_indices_flag) {
    mxArray *mat_segment = mxCreateStructMatrix(1, 1, SEGMENT_NUMFIELDS, SEGMENT_FIELDNAMES);
    map_mef3_segment_tostruct(segment, map_indices_flag, mat_segment, 0);
    return mat_segment;
}


/**
 *     Map a MEF channel c-struct to an exising matlab-struct
 *
 *    The given matlab-struct have multiple entries, the MEF
 *    struct will be mapped on the given index
 *
 *
 *    note: this funtion also loops through segments
 *
 *     @param channel            A pointer to the MEF channel c-struct
 *     @param map_indices_flag
 *     @param mat_channel        A pointer to the existing matlab-struct
 *     @param mat_index        The index in the existing matlab-struct at which to map the data
 */
void map_mef3_channel_tostruct(CHANNEL *channel, si1 map_indices_flag, mxArray *mat_channel, int mat_index) {
    si4       i;
    SEGMENT *segment;

    // set channel specific properties
    mxSetField(mat_channel, mat_index, "channel_type",                 mxInt32ByValue(channel->channel_type));
    mxSetField(mat_channel, mat_index, "number_of_segments",         mxInt64ByValue(channel->number_of_segments));
    mxSetField(mat_channel, mat_index, "path",                         mxCreateString(channel->path));
    mxSetField(mat_channel, mat_index, "name",                         mxCreateString(channel->name));
    mxSetField(mat_channel, mat_index, "extension",                 mxCreateString(channel->extension));
    mxSetField(mat_channel, mat_index, "session_name",                 mxCreateString(channel->session_name));
    mxSetField(mat_channel, mat_index, "level_UUID",                 mxUint8ArrayByValue(channel->level_UUID, UUID_BYTES));
    mxSetField(mat_channel, mat_index, "anonymized_name",             mxCreateString(channel->anonymized_name));
    mxSetField(mat_channel, mat_index, "maximum_number_of_records", mxInt64ByValue(channel->maximum_number_of_records));
    mxSetField(mat_channel, mat_index, "maximum_record_bytes",         mxInt64ByValue(channel->maximum_record_bytes));
    mxSetField(mat_channel, mat_index, "earliest_start_time",         mxInt64ByValue(channel->earliest_start_time));
    mxSetField(mat_channel, mat_index, "latest_end_time",             mxInt64ByValue(channel->latest_end_time));
    
    
    //
    // records
    //
    
    // check channel records and (if existent) add
    if ((channel->record_indices_fps != NULL) & (channel->record_data_fps != NULL)) {
        mxSetField(mat_channel, mat_index, "records", map_mef3_records(channel->record_indices_fps, channel->record_data_fps));
    }
    
    
    //
    // metadata
    //
    
    // create a metadata struct and assign it to the 'metadata' field
    mxArray *channel_metadata_struct = mxCreateStructMatrix(1, 1, METADATA_NUMFIELDS, METADATA_FIELDNAMES);
    mxSetField(mat_channel, mat_index, "metadata", channel_metadata_struct);
    
    // map channel metadata sections
    mxSetField(channel_metadata_struct, 0, "section_1", map_mef3_md1(channel->metadata.section_1));
    switch (channel->channel_type){
        case TIME_SERIES_CHANNEL_TYPE:
            mxSetField(channel_metadata_struct, 0, "section_2", map_mef3_tmd2(channel->metadata.time_series_section_2));
            break;
        case VIDEO_CHANNEL_TYPE:
            mxSetField(channel_metadata_struct, 0, "section_2", map_mef3_vmd2(channel->metadata.video_section_2));
            break;
        default:
            mexErrMsgTxt("Unrecognized channel type, exiting...");
    }
    mxSetField(channel_metadata_struct, 0, "section_3", map_mef3_md3(channel->metadata.section_3));
    
    // check if there are segments for this channel
    if (channel->number_of_segments > 0) {
        
        // create a segments struct
        mxArray *segmentsStruct = mxCreateStructMatrix(1, channel->number_of_segments, SEGMENT_NUMFIELDS, SEGMENT_FIELDNAMES);
        
        // map the segments
        for (i = 0; i < channel->number_of_segments; ++i) {
            segment = channel->segments + i;
            map_mef3_segment_tostruct(segment, map_indices_flag, segmentsStruct, i);
        }
        
        // assign the channels matlab-struct to the 'segments' field
        mxSetField(mat_channel, mat_index, "segments", segmentsStruct);
        
    }
    
}
 
/**
 *     Map a MEF channel c-struct to a newly created matlab-struct
 *
 *    note: this funtion also loops through segments
 *
 *     @param channel            A pointer to the MEF channel c-struct
 *     @param map_indices_flag
 *     @return                    A pointer to the new matlab-struct
 */
mxArray *map_mef3_channel(CHANNEL *channel, si1 map_indices_flag) {
    mxArray *mat_channel = mxCreateStructMatrix(1, 1, CHANNEL_NUMFIELDS, CHANNEL_FIELDNAMES);
    map_mef3_channel_tostruct(channel, map_indices_flag, mat_channel, 0);
    return mat_channel;
}

/**
 *     Map a MEF session c-struct to a newly created matlab-struct
 *
 *     @param session            A pointer to the MEF session c-struct
 *     @param map_indices_flag
 *     @return                    A pointer to the new matlab-struct
 */
mxArray *map_mef3_session(SESSION *session, si1 map_indices_flag) {
    si4       i;
    CHANNEL *channel;
    
    // create the a matlab 'session' struct
    mxArray *mat_session = mxCreateStructMatrix(1, 1, SESSION_NUMFIELDS, SESSION_FIELDNAMES);
    
    // set session-specific properties
    mxSetField(mat_session, 0, "number_of_time_series_channels",     mxInt32ByValue(session->number_of_time_series_channels));
    mxSetField(mat_session, 0, "number_of_video_channels",            mxInt32ByValue(session->number_of_video_channels));
    
    mxSetField(mat_session, 0, "name",                                 mxCreateString(session->name));
    mxSetField(mat_session, 0, "path",                                 mxCreateString(session->path));
    mxSetField(mat_session, 0, "anonymized_name",                     mxCreateString(session->anonymized_name));
    mxSetField(mat_session, 0, "level_UUID",                         mxUint8ArrayByValue(session->level_UUID, UUID_BYTES));
    mxSetField(mat_session, 0, "maximum_number_of_records",         mxInt64ByValue(session->maximum_number_of_records));
    mxSetField(mat_session, 0, "maximum_record_bytes",                 mxInt64ByValue(session->maximum_record_bytes));
    mxSetField(mat_session, 0, "earliest_start_time",                 mxInt64ByValue(session->earliest_start_time));
    mxSetField(mat_session, 0, "latest_end_time",                     mxInt64ByValue(session->latest_end_time));
    
    
    //
    // records
    //
    
    // check session records and (if existent) add
    if ((session->record_indices_fps != NULL) & (session->record_data_fps != NULL)) {
        mxSetField(mat_session, 0, "records", map_mef3_records(session->record_indices_fps, session->record_data_fps));
    }
    
    
    //
    // time_series
    //
    
    // check if there are time series channels
    if (session->number_of_time_series_channels > 0) {
        
        //
        // time_series_metadata
        //
        
        // create a metadata struct and assign it to the 'time_series_metadata' field
        mxArray *time_series_metadata_struct = mxCreateStructMatrix(1, 1, METADATA_NUMFIELDS, METADATA_FIELDNAMES);
        mxSetField(mat_session, 0, "time_series_metadata", time_series_metadata_struct);
        
        // map metadata sections
        mxSetField(time_series_metadata_struct, 0, "section_1",     map_mef3_md1(session->time_series_metadata.section_1));
        mxSetField(time_series_metadata_struct, 0, "section_2",     map_mef3_tmd2(session->time_series_metadata.time_series_section_2));
        mxSetField(time_series_metadata_struct, 0, "section_3",     map_mef3_md3(session->time_series_metadata.section_3));
        
        
        //
        // time_series_channels
        //
        
        
        // create a channels struct
        mxArray *channels_struct = mxCreateStructMatrix(1, session->number_of_time_series_channels, CHANNEL_NUMFIELDS, CHANNEL_FIELDNAMES);
        
        // map the time-serie channels
        for (i = 0; i < session->number_of_time_series_channels; ++i) {
            channel = session->time_series_channels + i;
            map_mef3_channel_tostruct(channel, map_indices_flag, channels_struct, i);
        }
        
        // assign the channels matlab-struct to the 'time_series_channels' field
        mxSetField(mat_session, 0, "time_series_channels", channels_struct);

    }
    
    
    //
    // video
    //
    
    // check if there are video channels
    if (session->number_of_video_channels > 0) {

        //
        // video_metadata
        //
    
        // create a metadata struct and assign it to the 'video_metadata' field
        mxArray *video_metadata_struct = mxCreateStructMatrix(1, 1, METADATA_NUMFIELDS, METADATA_FIELDNAMES);
        mxSetField(mat_session, 0, "video_metadata", video_metadata_struct);
        
        // map metadata sections
        mxSetField(video_metadata_struct, 0, "section_1",         map_mef3_md1(session->video_metadata.section_1));
        mxSetField(video_metadata_struct, 0, "section_2",        map_mef3_vmd2(session->video_metadata.video_section_2));
        mxSetField(video_metadata_struct, 0, "section_3",         map_mef3_md3(session->video_metadata.section_3));
        
        
        //
        // video_channels
        //
        
        // create a channels struct
        mxArray *channels_struct = mxCreateStructMatrix(1, session->number_of_video_channels, CHANNEL_NUMFIELDS, CHANNEL_FIELDNAMES);
        
        // map the video channels
        for (i = 0; i < session->number_of_video_channels; ++i) {
            channel = session->video_channels + i;
            map_mef3_channel_tostruct(channel, map_indices_flag, channels_struct, i);
        }
        
        // assign the channels matlab-struct to the 'video_channels' field
        mxSetField(mat_session, 0, "video_channels", channels_struct);
        
    }
    
    // return the output-struct
    return mat_session;
    
}

/**
 *     Map a MEF section 1 metadata c-struct to a newly created matlab-struct
 *
 *     @param md1            A pointer to the MEF section 1 metadata c-struct
 *     @return                A pointer to the new matlab-struct
 */
mxArray *map_mef3_md1(METADATA_SECTION_1 *md1) {

    mxArray *mat_md = mxCreateStructMatrix(1, 1, METADATA_SECTION_1_NUMFIELDS, METADATA_SECTION_1_FIELDNAMES);
    mxSetField(mat_md, 0, "section_2_encryption",             mxInt8ByValue(md1->section_2_encryption));
    mxSetField(mat_md, 0, "section_3_encryption",             mxInt8ByValue(md1->section_3_encryption));

    // return the struct
    return mat_md;
}

/**
 *     Map a MEF section 2 time-series metadata c-struct to a newly created matlab-struct
 *
 *     @param tmd2            A pointer to the MEF section 2 time-series metadata c-struct
 *     @return                A pointer to the new matlab-struct
 */
mxArray *map_mef3_tmd2(TIME_SERIES_METADATA_SECTION_2 *tmd2) {

    mxArray *mat_md = mxCreateStructMatrix(1, 1, TIME_SERIES_METADATA_SECTION_2_NUMFIELDS, TIME_SERIES_METADATA_SECTION_2_FIELDNAMES);
    
    
    mxSetField(mat_md, 0, "channel_description",             mxCreateString(tmd2->channel_description));
    mxSetField(mat_md, 0, "session_description",             mxCreateString(tmd2->session_description));
    
    mxSetField(mat_md, 0, "recording_duration",             mxInt64ByValue(tmd2->recording_duration));
    mxSetField(mat_md, 0, "reference_description",             mxCreateString(tmd2->reference_description));
    mxSetField(mat_md, 0, "acquisition_channel_number",     mxInt64ByValue(tmd2->acquisition_channel_number));
    mxSetField(mat_md, 0, "sampling_frequency",             mxDoubleByValue(tmd2->sampling_frequency));
    mxSetField(mat_md, 0, "low_frequency_filter_setting",     mxDoubleByValue(tmd2->low_frequency_filter_setting));
    mxSetField(mat_md, 0, "high_frequency_filter_setting",     mxDoubleByValue(tmd2->high_frequency_filter_setting));
    mxSetField(mat_md, 0, "notch_filter_frequency_setting", mxDoubleByValue(tmd2->notch_filter_frequency_setting));
    mxSetField(mat_md, 0, "AC_line_frequency",                 mxDoubleByValue(tmd2->AC_line_frequency));
    mxSetField(mat_md, 0, "units_conversion_factor",         mxDoubleByValue(tmd2->units_conversion_factor));
    mxSetField(mat_md, 0, "units_description",                 mxCreateString(tmd2->units_description));
    mxSetField(mat_md, 0, "maximum_native_sample_value",     mxDoubleByValue(tmd2->maximum_native_sample_value));
    mxSetField(mat_md, 0, "minimum_native_sample_value",     mxDoubleByValue(tmd2->minimum_native_sample_value));
    mxSetField(mat_md, 0, "start_sample",                     mxInt64ByValue(tmd2->start_sample));
    mxSetField(mat_md, 0, "number_of_samples",                 mxInt64ByValue(tmd2->number_of_samples));
    mxSetField(mat_md, 0, "number_of_blocks",                 mxInt64ByValue(tmd2->number_of_blocks));
    mxSetField(mat_md, 0, "maximum_block_bytes",             mxInt64ByValue(tmd2->maximum_block_bytes));
    mxSetField(mat_md, 0, "maximum_block_samples",             mxUint32ByValue(tmd2->maximum_block_samples));
    mxSetField(mat_md, 0, "maximum_difference_bytes",         mxUint32ByValue(tmd2->maximum_difference_bytes));
    mxSetField(mat_md, 0, "block_interval",                 mxInt64ByValue(tmd2->block_interval));
    mxSetField(mat_md, 0, "number_of_discontinuities",         mxInt64ByValue(tmd2->number_of_discontinuities));
    mxSetField(mat_md, 0, "maximum_contiguous_blocks",         mxInt64ByValue(tmd2->maximum_contiguous_blocks));
    mxSetField(mat_md, 0, "maximum_contiguous_block_bytes", mxInt64ByValue(tmd2->maximum_contiguous_block_bytes));
    mxSetField(mat_md, 0, "maximum_contiguous_samples",     mxInt64ByValue(tmd2->maximum_contiguous_samples));
    
    // return the struct
    return mat_md;
}

/**
 *     Map a MEF section 2 video-series metadata c-struct to a newly created matlab-struct
 *
 *     @param vmd2            A pointer to the MEF section 2 video-series metadata c-struct
 *     @return                A pointer to the new matlab-struct
 */
mxArray *map_mef3_vmd2(VIDEO_METADATA_SECTION_2 *vmd2) {

    mxArray *mat_md = mxCreateStructMatrix(1, 1, VIDEO_METADATA_SECTION_2_NUMFIELDS, VIDEO_METADATA_SECTION_2_FIELDNAMES);
    
    mxSetField(mat_md, 0, "channel_description",             mxCreateString(vmd2->channel_description));
    mxSetField(mat_md, 0, "session_description",             mxCreateString(vmd2->session_description));
    
    mxSetField(mat_md, 0, "recording_duration",             mxInt64ByValue(vmd2->recording_duration));
    mxSetField(mat_md, 0, "horizontal_resolution",             mxInt64ByValue(vmd2->horizontal_resolution));
    mxSetField(mat_md, 0, "vertical_resolution",             mxInt64ByValue(vmd2->vertical_resolution));
    mxSetField(mat_md, 0, "frame_rate",                     mxDoubleByValue(vmd2->frame_rate));
    mxSetField(mat_md, 0, "number_of_clips",                 mxInt64ByValue(vmd2->number_of_clips));
    mxSetField(mat_md, 0, "maximum_clip_bytes",             mxInt64ByValue(vmd2->maximum_clip_bytes));
    mxSetField(mat_md, 0, "video_format",                     mxCreateString(vmd2->video_format));
    //mxSetField(mat_md, 0, "video_file_CRC",                 mxUint32ByValue(vmd2->video_file_CRC));            // TODO: check with valid value, leave empty for now
    
    // return the struct
    return mat_md;
}

/**
 *     Map a MEF section 3 metadata c-struct to a newly created matlab-struct
 *
 *     @param md3            A pointer to the MEF section 3 metadata c-struct
 *     @return                A pointer to the new matlab-struct
 */
mxArray *map_mef3_md3(METADATA_SECTION_3 *md3) {

    mxArray *mat_md = mxCreateStructMatrix(1, 1, METADATA_SECTION_3_NUMFIELDS, METADATA_SECTION_3_FIELDNAMES);
    
    mxSetField(mat_md, 0, "recording_time_offset",             mxInt64ByValue(md3->recording_time_offset));
    mxSetField(mat_md, 0, "DST_start_time",                 mxInt64ByValue(md3->DST_start_time));
    mxSetField(mat_md, 0, "DST_end_time",                     mxInt64ByValue(md3->DST_end_time));
    mxSetField(mat_md, 0, "GMT_offset",                     mxInt32ByValue(md3->GMT_offset));
    mxSetField(mat_md, 0, "subject_name_1",                 mxCreateString(md3->subject_name_1));
    mxSetField(mat_md, 0, "subject_name_2",                 mxCreateString(md3->subject_name_2));
    mxSetField(mat_md, 0, "subject_ID",                     mxCreateString(md3->subject_ID));
    mxSetField(mat_md, 0, "recording_location",             mxCreateString(md3->recording_location));
    
    // return the struct
    return mat_md;
}


mxArray *map_mef3_ti(TIME_SERIES_INDEX *ti, si8 number_of_entries) {

    // create the a matlab 'time_series_index' struct
    mxArray *mat_ti = mxCreateStructMatrix(1, number_of_entries, TIME_SERIES_INDEX_NUMFIELDS, TIME_SERIES_INDEX_FIELDNAMES);
    
    // loop through the time-series indices
    si8 i;
    for (i = 0; i < number_of_entries; ++i) {
        TIME_SERIES_INDEX *cur_ti = ti + i;
        
        mxSetField(mat_ti, i, "file_offset",                 mxInt64ByValue(cur_ti->file_offset));
        mxSetField(mat_ti, i, "start_time",                 mxInt64ByValue(cur_ti->start_time));
        mxSetField(mat_ti, i, "start_sample",                 mxInt64ByValue(cur_ti->start_sample));
        mxSetField(mat_ti, i, "number_of_samples",             mxUint32ByValue(cur_ti->number_of_samples));
        mxSetField(mat_ti, i, "block_bytes",                 mxUint32ByValue(cur_ti->block_bytes));
        mxSetField(mat_ti, i, "maximum_sample_value",         mxInt32ByValue(cur_ti->maximum_sample_value));
        mxSetField(mat_ti, i, "minimum_sample_value",         mxInt32ByValue(cur_ti->minimum_sample_value));
        mxSetField(mat_ti, i, "RED_block_flags",             mxUint8ByValue(cur_ti->RED_block_flags));
    }
    
    // return the struct
    return mat_ti;
}


mxArray *map_mef3_vi(VIDEO_INDEX *vi, si8 number_of_entries) {

    // create the a matlab 'video_index' struct
    mxArray *mat_vi = mxCreateStructMatrix(1, number_of_entries, VIDEO_INDEX_NUMFIELDS, VIDEO_INDEX_FIELDNAMES);
    
    // loop through the video indices
    si8 i;
    for (i = 0; i < number_of_entries; ++i) {
        VIDEO_INDEX *cur_vi = vi + i;
        
        mxSetField(mat_vi, i, "start_time",                 mxInt64ByValue(cur_vi->start_time));
        mxSetField(mat_vi, i, "end_time",                     mxInt64ByValue(cur_vi->end_time));
        mxSetField(mat_vi, i, "start_frame",                 mxUint32ByValue(cur_vi->start_frame));
        mxSetField(mat_vi, i, "end_frame",                     mxUint32ByValue(cur_vi->end_frame));
        mxSetField(mat_vi, i, "file_offset",                 mxInt64ByValue(cur_vi->file_offset));
        mxSetField(mat_vi, i, "clip_bytes",                 mxInt64ByValue(cur_vi->clip_bytes));
        
    }
    
    // return the struct
    return mat_vi;
}


/**
 *     Map the MEF records to a matlab-struct
 *
 *     @param ri_fps
 *     @param rd_fps
 */
mxArray *map_mef3_records(FILE_PROCESSING_STRUCT *ri_fps, FILE_PROCESSING_STRUCT *rd_fps) {
    si4 i;
    RECORD_HEADER *rh;
    ui4     *type_str_int;
    ui4     type_code;
    
    // retrieve the number of records
    si8 number_of_records = ri_fps->universal_header->number_of_entries;

    // create custom record struct list
    const int RECORD_NUMFIELDS        = 5;
    const char *RECORD_FIELDNAMES[]    = {
        "time",
        "type",
        "version_major",
        "version_minor",
        "body"
    };
    mxArray *mat_records = mxCreateStructMatrix(1, number_of_records, RECORD_NUMFIELDS, RECORD_FIELDNAMES);
    
    // point to first record entry
    ui1 *rd = rd_fps->raw_data + UNIVERSAL_HEADER_BYTES;
    
    // loop through the records
    for (i = 0; i < number_of_records; ++i) {
        
        // cast the record header
        rh = (RECORD_HEADER *) rd;


        //
        // header
        //
        mxSetField(mat_records, i, "time", mxInt64ByValue(rh->time));
        mxSetField(mat_records, i, "type", mxCreateString(rh->type_string));
        mxSetField(mat_records, i, "version_major", mxUint8ByValue(rh->version_major));
        mxSetField(mat_records, i, "version_minor", mxUint8ByValue(rh->version_minor));
        
        
        //
        // body
        //
        type_str_int = (ui4 *) rh->type_string;
        type_code = *type_str_int;
        switch (type_code) {
            case MEFREC_Note_TYPE_CODE:
                if (rh->version_major == 1 && rh->version_minor == 0) {
                    si1 *note_p = (si1 *) rh + MEFREC_Note_1_0_TEXT_OFFSET;
                    mxSetField(mat_records, i, "body", mxCreateString(note_p));
                }
                else
                    mexPrintf("Warning: unrecognized Note version, skipping note body\n");
                break;
            case MEFREC_EDFA_TYPE_CODE:
                // TODO: need test
                if (rh->version_major == 1 && rh->version_minor == 0) {
                    MEFREC_EDFA_1_0 *edfa_p = (MEFREC_EDFA_1_0 *)((ui1 *)rh + MEFREC_EDFA_1_0_OFFSET);
                    si1 *annotation_p = (si1 *)((ui1 *)rh+MEFREC_EDFA_1_0_ANNOTATION_OFFSET);
                    mxArray *mat_edfa = mxCreateStructMatrix(1,1,MEFREC_EDFA_1_0_NUMFIELDS,MEFREC_EDFA_1_0_FIELDNAMES);
                    
                    mxSetField(mat_edfa,0,"duration",mxInt64ByValue(edfa_p->duration));
                    mxSetField(mat_edfa,0,"annotation",mxCreateString(annotation_p));
                    
                    mxSetField(mat_records,i,"body",mat_edfa);
                }
                else
                    mexPrintf("Warning: unrecognized EDF Annotation (EDFA) version, skipping EDFA body\n");
                break;
            case MEFREC_LNTP_TYPE_CODE:
                // TODO: need test
                if(rh->version_major == 1 && rh->version_minor == 0)
                {
                    MEFREC_LNTP_1_0 *lntp_p = (MEFREC_LNTP_1_0 *)((ui1 *)rh+MEFREC_LNTP_1_0_OFFSET);
                    si4 *template_p = (si4 *)((ui1 *)rh+MEFREC_LNTP_1_0_TEMPLATE_OFFSET);
                    mxArray *mat_lntp = mxCreateStructMatrix(1,1,MEFREC_LNTP_1_0_NUMFIELDS,MEFREC_LNTP_1_0_FIELDNAMES);
                    
                    mxSetField(mat_lntp,0,"length",mxInt64ByValue(lntp_p->length));
                    mxSetField(mat_lntp,0,"template",mxInt32ArrayByValue(template_p,(int)mxInt64ByValue(lntp_p->length)));
                    
                    mxSetField(mat_records,i,"body",mat_lntp);
                }
                else
                    mexPrintf("Warning: unrecognized Line Noise Template (LNTP) version, skipping LNTP body\n");
                break;
            case MEFREC_Seiz_TYPE_CODE:
                // TODO: need test
                if(rh->version_major == 1 && rh->version_minor == 0){
                    MEFREC_Seiz_1_0 *seiz_p = (MEFREC_Seiz_1_0 *)((ui1 *)rh+MEFREC_Seiz_1_0_OFFSET);
                    MEFREC_Seiz_1_0_CHANNEL *seiz_channel_p = (MEFREC_Seiz_1_0_CHANNEL *)((ui1 *)rh+MEFREC_Seiz_1_0_CHANNELS_OFFSET);
                    mxArray *mat_seiz = mxCreateStructMatrix(1,1,MEFREC_SEIZ_1_0_NUMFIELDS,MEFREC_SEIZ_1_0_FIELDNAMES);
                    mxArray *mat_seiz_channel = mxCreateStructMatrix(1,1,MEFREC_SEIZ_1_0_CHANNEL_NUMFIELDS,MEFREC_SEIZ_1_0_CHANNEL_FIELDNAMES);
                    mxArray *mat_seiz_body = mxCreateStructMatrix(1,1,MEFREC_SEIZ_1_0_BODY_NUMFIELDS,MEFREC_SEIZ_1_0_BODY_FIELDNAMES);
                    
                    // set fields of Seiz structure
                    mxSetField(mat_seiz,0,"earliest_onset",mxInt64ByValue(seiz_p->earliest_onset));
                    mxSetField(mat_seiz,0,"latest_offset",mxInt64ByValue(seiz_p->latest_offset));
                    mxSetField(mat_seiz,0,"duration",mxInt64ByValue(seiz_p->duration));
                    mxSetField(mat_seiz,0,"number_of_channels",mxInt32ByValue(seiz_p->number_of_channels));
                    mxSetField(mat_seiz,0,"onset_code",mxInt32ByValue(seiz_p->onset_code));
                    mxSetField(mat_seiz,0,"marker_name_1",mxCreateString(seiz_p->marker_name_1));
                    mxSetField(mat_seiz,0,"marker_name_2",mxCreateString(seiz_p->marker_name_2));
                    mxSetField(mat_seiz,0,"annotation",mxCreateString(seiz_p->annotation));
                    
                    // set fields of Seiz_CHANNEL
                    mxSetField(mat_seiz_channel,0,"onset",mxInt64ByValue(seiz_channel_p->onset));
                    mxSetField(mat_seiz_channel,0,"offset",mxInt64ByValue(seiz_channel_p->offset));
                    
                    // set fields of seiz_body
                    mxSetField(mat_seiz_body,0,"seizure",mat_seiz_body);
                    mxSetField(mat_seiz_body,0,"channel",mat_seiz_channel);
                    
                    mxSetField(mat_records,i,"body",mat_seiz_body);
                }
                else
                    mexPrintf("Warning: unrecognized Seizure (Siez) version, skipping Seiz body\n");
                break;
            case MEFREC_CSti_TYPE_CODE:
                if(rh->version_major == 1 && rh->version_minor == 0){
                    MEFREC_CSti_1_0 *csti_p = (MEFREC_CSti_1_0 *)((ui1 *)rh+MEFREC_CSti_1_0_OFFSET);
                    mxArray *mat_csti = mxCreateStructMatrix(1,1,MEFREC_CSTI_1_0_NUMFIELDS,MEFREC_CSTI_1_0_FIELDNAMES);
                    
                    mxSetField(mat_csti,0,"task_type",mxCreateString(csti_p->task_type));
                    mxSetField(mat_csti,0,"stimulus_duration",mxInt64ByValue(csti_p->stimulus_duration));
                    mxSetField(mat_csti,0,"stimulus_type",mxCreateString(csti_p->stimulus_type));
                    mxSetField(mat_csti,0,"patient_response",mxCreateString(csti_p->patient_response));

                    mxSetField(mat_records,i,"body",mat_csti);
                }
                else
                    mexPrintf("Warning: unrecognized Cognitive Stimulation (CSti) version, skipping CSti body\n");
                break;
            case MEFREC_ESti_TYPE_CODE:
                // TODO: need test
                if(rh->version_major == 1 && rh->version_minor == 0){
                    MEFREC_ESti_1_0 *esti_p = (MEFREC_ESti_1_0 *)((ui1 *)rh+MEFREC_ESti_1_0_OFFSET);
                    mxArray *mat_esti = mxCreateStructMatrix(1,1,MEFREC_ESTI_1_0_NUMFIELDS,MEFREC_ESTI_1_0_FIELDNAMES);
                    
                    mxSetField(mat_esti,0,"amplitude",mxInt64ByValue(esti_p->amplitude));
                    mxSetField(mat_esti,0,"frequency",mxInt64ByValue(esti_p->frequency));
                    mxSetField(mat_esti,0,"pulse_width",mxInt64ByValue(esti_p->pulse_width));
                    mxSetField(mat_esti,0,"ampunit_code",mxInt32ByValue(esti_p->mode_code));
                    mxSetField(mat_esti,0,"mode_code",mxInt32ByValue(esti_p->mode_code));
                    mxSetField(mat_esti,0,"waveform",mxCreateString(esti_p->waveform));
                    mxSetField(mat_esti,0,"anode",mxCreateString(esti_p->anode));
                    mxSetField(mat_esti,0,"catode",mxCreateString(esti_p->catode));
                    
                    mxSetField(mat_records,i,"body",mat_esti);
                }
                else
                    mexPrintf("Warning: unrecognized Electric Stimulation (ESti) version, skipping ESti body\n");
                break;
            case MEFREC_SyLg_TYPE_CODE:
                if (rh->version_major == 1 && rh->version_minor == 0) {
                    si1 *SyLg_p = (si1 *) rh + MEFREC_SyLg_1_0_TEXT_OFFSET;
                    mxSetField(mat_records, i, "body", mxCreateString(SyLg_p));
                }
                else
                    mexPrintf("Warning: unrecognized System Log (SyLg) version, skipping SyLg body\n");
                break;
            case MEFREC_UnRc_TYPE_CODE:
                mexPrintf("Error: \"%s\" (0x%x) is an unrecognized record type\n", rh->type_string, type_code);
                return NULL;
            default:
                mexPrintf("Error: \"%s\" (0x%x) is an unrecognized record type\n", rh->type_string, type_code);
                return NULL;
        }
        
        // forward the record pointer to the next record
        rd += (RECORD_HEADER_BYTES + rh->bytes);
        
    }
    
    // return the struct
    return mat_records;
}

/**
 *     Map a MEF universal_header c-struct to a newly created matlab-struct
 *  (added by Richard J. Cui)
 *
 *     @param universal_header        Pointer to the MEF universal_header c-struct
 *     @return                        Pointer to the new matlab-struct
 */
mxArray *map_mef3_uh(UNIVERSAL_HEADER *universal_header) {
    
    mxArray *mat_uh = mxCreateStructMatrix(1, 1, UNIVERSAL_HEADER_NUMFIELDS, UNIVERSAL_HEADER_FIELDNAMES);
    
    mxSetField(mat_uh, 0, "header_CRC",                     mxUint32ByValue(universal_header->header_CRC));
    mxSetField(mat_uh, 0, "body_CRC",                         mxUint32ByValue(universal_header->body_CRC));
    mxSetField(mat_uh, 0, "file_type_string",                 mxCreateString(universal_header->file_type_string));
    mxSetField(mat_uh, 0, "mef_version_major",                 mxUint8ByValue(universal_header->mef_version_major));
    mxSetField(mat_uh, 0, "mef_version_minor",                 mxUint8ByValue(universal_header->mef_version_minor));
    mxSetField(mat_uh, 0, "byte_order_code",                 mxUint8ByValue(universal_header->byte_order_code));
    mxSetField(mat_uh, 0, "start_time",                     mxInt64ByValue(universal_header->start_time));
    mxSetField(mat_uh, 0, "end_time",                         mxInt64ByValue(universal_header->end_time));
    mxSetField(mat_uh, 0, "number_of_entries",                 mxInt64ByValue(universal_header->number_of_entries));
    mxSetField(mat_uh, 0, "maximum_entry_size",             mxInt64ByValue(universal_header->maximum_entry_size));
    mxSetField(mat_uh, 0, "segment_number",                 mxInt32ByValue(universal_header->segment_number));
    mxSetField(mat_uh, 0, "channel_name",                     mxCreateString(universal_header->channel_name));
    mxSetField(mat_uh, 0, "session_name",                     mxCreateString(universal_header->session_name));
    mxSetField(mat_uh, 0, "anonymized_name",                 mxCreateString(universal_header->anonymized_name));
    mxSetField(mat_uh, 0, "level_UUID",                     mxUint8ArrayByValue(universal_header->level_UUID, UUID_BYTES));
    mxSetField(mat_uh, 0, "file_UUID",                         mxUint8ArrayByValue(universal_header->file_UUID, UUID_BYTES));
    mxSetField(mat_uh, 0, "provenance_UUID",                 mxUint8ArrayByValue(universal_header->provenance_UUID, UUID_BYTES));
    mxSetField(mat_uh, 0, "level_1_password_validation_field", mxUint8ArrayByValue(universal_header->level_1_password_validation_field, PASSWORD_VALIDATION_FIELD_BYTES));
    mxSetField(mat_uh, 0, "level_2_password_validation_field", mxUint8ArrayByValue(universal_header->level_2_password_validation_field, PASSWORD_VALIDATION_FIELD_BYTES));
    mxSetField(mat_uh, 0, "protected_region",                 mxUint8ArrayByValue(universal_header->protected_region, UNIVERSAL_HEADER_PROTECTED_REGION_BYTES));
    mxSetField(mat_uh, 0, "discretionary_region",             mxUint8ArrayByValue(universal_header->discretionary_region, UNIVERSAL_HEADER_DISCRETIONARY_REGION_BYTES));
    
    // return the struct
    return mat_uh;
    
}

/**
 * Create a (1xN real) Uint8 vector/matrix based on a MEF array of ui1 (unsigned 1 byte int) values
 * (by Richard J. Cui)
 *
 * @param array            The array to store in the matlab variable
 * @param num_bytes        The number of values in the array to transfer
 * @return                The mxArray containing the array
 */
mxArray *mxUint8ArrayByValue(ui1 *array, int num_bytes) {
    int i;
    
    // create the matlab variable (1x1 real double matrix)
    mxArray *retArr = mxCreateNumericMatrix(1, num_bytes, mxUINT8_CLASS, mxREAL);
    
    // transfer the values to the matlab (allocated memory)
    unsigned char *ucp = (unsigned char *)mxGetData(retArr);
    for (i = 0; i < num_bytes; i++) ucp[i] = array[i];
    
    // return the matlab variable
    return retArr;
}

// creat a 1 x N Int vector/matrix based on a MEF array of si4 (int) values
mxArray *mxInt32ArrayByValue(si4 *array, int num_ints) {
    int i;
    
    // create the matlab variable (1x1 real double matrix)
    mxArray *retArr = mxCreateNumericMatrix(1, num_ints, mxUINT32_CLASS, mxREAL);
    
    // transfer the values to the matlab (allocated memory)
    si4 *int_p = (si4 *)mxGetData(retArr);
    for (i = 0; i < num_ints; i++)
        int_p[i] = array[i];
    
    // return the matlab variable
    return retArr;
}

/**
 * Create a (1x1 real) Uint8 matrix based on a MEF ui1 (unsigned 1 byte int) variable
 *
 * @param value        The value to store in the matlab variable
 * @return            The mxArray containing the value
 */
mxArray *mxUint8ByValue(ui1 value) {
    
    // create the matlab variable (1x1 real uint8 matrix)
    mxArray *retArr = mxCreateNumericMatrix(1, 1, mxUINT8_CLASS, mxREAL);
    
    // retrieve the pointer to the memory allocated by matlab
    mxUint8 *data = mxGetData(retArr);
    
    // transfer the value to the matlab (allocated memory)
    data[0] = (mxUint8)value;
    
    // return the matlab variable
    return retArr;
    
}

/**
 * Create a (1x1 real) Int8 matrix based on a MEF si1 (signed 1 byte int) variable
 *
 * @param value        The value to store in the matlab variable
 * @return            The mxArray containing the value
 */
mxArray *mxInt8ByValue(si1 value) {
    
    // create the matlab variable (1x1 real int8 matrix)
    mxArray *retArr = mxCreateNumericMatrix(1, 1, mxINT8_CLASS, mxREAL);
    
    // retrieve the pointer to the memory allocated by matlab
    mxInt8 *data = mxGetData(retArr);
    
    // transfer the value to the matlab (allocated memory)
    data[0] = (mxInt8)value;
    
    // return the matlab variable
    return retArr;
    
}


/**
 * Create a (1x1 real) Uint32 matrix based on a MEF ui4 (unsigned 4 byte int) variable
 *
 * @param value        The value to store in the matlab variable
 * @return            The mxArray containing the value
 */
mxArray *mxUint32ByValue(ui4 value) {
    
    // create the matlab variable (1x1 real uint32 matrix)
    mxArray *retArr = mxCreateNumericMatrix(1, 1, mxUINT32_CLASS, mxREAL);
    
    // retrieve the pointer to the memory allocated by matlab
    mxUint32 *data = (mxUint32 *)mxGetData(retArr);
    
    // transfer the value to the matlab (allocated memory)
    data[0] = (mxUint32)value;
    
    // return the double variable
    return retArr;
    
}


/**
 * Create a (1x1 real) Int32 matrix based on a MEF si4 (signed 4 byte int) variable
 *
 * @param value        The value to store in the matlab variable
 * @return            The mxArray containing the value
 */
mxArray *mxInt32ByValue(si4 value) {
    
    // create the matlab variable (1x1 real int32 matrix)
    mxArray *retArr = mxCreateNumericMatrix(1, 1, mxINT32_CLASS, mxREAL);
    
    // retrieve the pointer to the memory allocated by matlab
    mxInt32 *data = mxGetData(retArr);
    
    // transfer the value to the matlab (allocated memory)
    data[0] = (mxInt32)value;
    
    // return the matlab variable
    return retArr;
    
}

/**
 * Create a (1x1 real) Uint64 matrix based on a MEF ui8 (unsigned 8 byte int) variable
 *
 * @param value        The value to store in the matlab variable
 * @return            The mxArray containing the value
 */
mxArray *mxUint64ByValue(ui8 value) {
    
    // create the matlab variable (1x1 real uint64 matrix)
    mxArray *retArr = mxCreateNumericMatrix(1, 1, mxUINT64_CLASS, mxREAL);
    
    // retrieve the pointer to the memory allocated by matlab
    mxUint64 *data = mxGetData(retArr);
    
    // transfer the value to the matlab (allocated memory)
    data[0] = (mxUint64)value;
    
    // return the matlab variable
    return retArr;
    
}

/**
 * Create a (1x1 real) Int64 matrix based on a MEF si8 (signed 8 byte int) variable
 *
 * @param value        The value to store in the matlab variable
 * @return            The mxArray containing the value
 */
mxArray *mxInt64ByValue(si8 value) {
    
    // create the matlab variable (1x1 real int64 matrix)
    mxArray *retArr = mxCreateNumericMatrix(1, 1, mxINT64_CLASS, mxREAL);
    
    // retrieve the pointer to the memory allocated by matlab
    mxInt64 *data = mxGetData(retArr);
    
    // transfer the value to the matlab (allocated memory)
    data[0] = (mxInt64)value;
    
    // return the matlab variable
    return retArr;
    
}


/**
 * Create a (1x1 real) double matrix based on a MEF sf8 (signed 8 byte float) variable
 *
 * @param value        The value to store in the matlab variable
 * @return            The mxArray containing the value
 */
mxArray *mxDoubleByValue(sf8 value) {
    
    // create the matlab variable (1x1 real double matrix)
    mxArray *retArr = mxCreateDoubleMatrix(1, 1, mxREAL);
    
    // retrieve the pointer to the memory allocated by matlab
    mxDouble *data = mxGetPr(retArr);
    
    // transfer the value to the matlab (allocated memory)
    data[0] = (mxDouble)value;
    
    // return the matlab variable
    return retArr;
    
}

/**
 * Create a matlab char string (defaulted to matlab's encoding, most likely UTF-16) based
 * on a MEF UTF-8 character string
 *
 * @param value        The value to store in the matlab variable
 * @return            The mxArray containing the value
 */
mxArray *mxStringByUTF8Value(char *str) {
    
    // TODO: try UTF-8 strings, function exists but need set to test
    
    // retrieve the number of bytes in the input string
    // note: this could differ from the actual number of characters according
    //       to UTF-8 (depending whether the string contains non-ASCII characters)
    int lengthInBytes = strlen(str);
    
    // copy the byte values from the input string into an matlab uint8 array
    // note: mxCreateString would do this, but also would destroy the non-ASCII
    //         code points (any byte with a value above 127 is converted to 65535)
    mxArray *mat_uint8 = mxCreateNumericMatrix(1, lengthInBytes, mxUINT8_CLASS, mxREAL);
    mxUint8 *p_mat_uint8 = mxGetData(mat_uint8);
    for (int i = 0; i < lengthInBytes; i++) {
        p_mat_uint8[i] = str[i];
    }
    
    // use the native2unicode call from matlab to convert the
    // UTF-8 input to the standard matlab coding (UTF-16)
    mxArray *lhs[1];
    mxArray *rhs[] = {mat_uint8, mxCreateString("UTF-8")};
    mexCallMATLAB(1, lhs, 2, rhs, "native2unicode");
    
    // free the memory
    mxFree(mat_uint8);
    
    // return matlab array (should now be UTF-16)
    return lhs[0];
    
}

//  the gate funciton
/**
 * Main entry point for 'read_mef_info_mex_3p9'
 *
 * @param sessionPath    Path (absolute or relative) to the MEF3 session folder
 * @param password        Password to the MEF3 data; Pass empty string/variable if not encrypted
 * @param mapIndices    Flag whether indices should be mapped [0 or 1; default is 0]
 * @return                Structure containing session metadata, channels metadata, segments metadata and records
 */
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    
    //
    // session path
    //
    
    // check the session path input argument
    if (nrhs < 1) {
        mexErrMsgIdAndTxt( "MATLAB:read_mef_info_mex_3p0:noSessionPathArg", "sessionPath input argument not set");
    } else {
        if (!mxIsChar(prhs[0])) {
            mexErrMsgIdAndTxt( "MATLAB:read_mef_info_mex_3p0:invalidSessionPathArg", "sessionPath input argument invalid, should string (array of characters)");
        }
        if (mxIsEmpty(prhs[0])) {
            mexErrMsgIdAndTxt( "MATLAB:read_mef_info_mex_3p0:invalidSessionPathArg", "sessionPath input argument invalid, argument is empty");
        }
    }
    
    // set the session path
    si1 session_path[MEF_FULL_FILE_NAME_BYTES];
    char *mat_session_path = mxArrayToString(prhs[0]);
    MEF_strncpy(session_path, mat_session_path, MEF_FULL_FILE_NAME_BYTES);
    
    
    //
    // password (optional)
    //
    
    si1 *password = NULL;
    si1 password_arr[PASSWORD_BYTES] = {0};
    
    // check if a password input argument is given
    if (nrhs > 1) {

        // note: if the password passed to any of the meflib read function is an empty string, than
        //         the 'process_password_data' function in 'meflib.c' will crash everything, so make
        //          sure it is either NULL or a string

        // check if the password input argument is not empty
        if (!mxIsEmpty(prhs[1])) {
                
            // check the password input argument data type
            if (!mxIsChar(prhs[1])) {
                mexErrMsgIdAndTxt( "MATLAB:read_mef_info_mex_3p0:invalidPasswordArg", "password input argument invalid, should string (array of characters)");
            }
            
            //char *mat_password = mxArrayToUTF8String(prhs[1]);
            char *mat_password = mxArrayToString(prhs[1]);
            password = strcpy(password_arr, mat_password);
    
        }
        
    }
    
    
    //
    // map indices (optional)
    //
    
    // Read indices flag
    si1 map_indices_flag = 0;
    
    // check if a map indices input argument is given
    if (nrhs > 2) {
        
        // check if not empty
        if (!mxIsEmpty(prhs[2])) {

            // check if single numeric or logical
            if ((!mxIsNumeric(prhs[2]) && !mxIsLogical(prhs[2])) || mxGetNumberOfElements(prhs[2]) > 1) {
                mexErrMsgIdAndTxt( "MATLAB:read_mef_info_mex_3p0:invalidMapIndicesArg", "mapIndices input argument invalid, should be a single value logical or numeric");
            }
            
            // retrieve the map indices flag value
            int mat_map_indices_flag = mxGetScalar(prhs[2]);
            
            // check the value
            if (mat_map_indices_flag != 0 && mat_map_indices_flag != 1) {
                mexErrMsgIdAndTxt( "MATLAB:read_mef_info_mex_3p0:invalidMapIndicesArg", "mapIndices input argument invalid, allowed values are 0, false, 1 or true");
            }
            
            // set the flag
            map_indices_flag = mat_map_indices_flag;
            
        }
        
    }
    
    
    //
    // read session metadata
    //
    
    // initialize MEF library
    initialize_meflib();

    // read the session metadata
    MEF_globals->behavior_on_fail = SUPPRESS_ERROR_OUTPUT;
    SESSION *session = read_MEF_session(    NULL,                     // allocate new session object
                                            session_path,             // session filepath
                                            password,                 // password
                                            NULL,                     // empty password
                                            MEF_FALSE,                 // do not read time series data
                                            MEF_TRUE                // read record data
                                        );
    MEF_globals->behavior_on_fail = EXIT_ON_FAIL;
    
    // check for error
    if (session == NULL)    mexErrMsgTxt("Error while reading session metadata");
    
    // check if the data is encrypted and/or the correctness of password
    if (session->time_series_metadata.section_1 != NULL) {
        if (session->time_series_metadata.section_1->section_2_encryption > 0 || session->time_series_metadata.section_1->section_2_encryption > 0) {
            free_session(session, MEF_TRUE);
            if (password == NULL)
                mexErrMsgTxt("Error: data is encrypted, but no password is given, exiting...\n");
            else
                mexErrMsgTxt("Error: wrong password for encrypted data, exiting...\n");
            
        }
    }
    if (session->video_metadata.section_1 != NULL) {
        if (session->video_metadata.section_1->section_2_encryption > 0 || session->video_metadata.section_1->section_2_encryption > 0) {
            free_session(session, MEF_TRUE);
            if (password == NULL)
                mexErrMsgTxt("Error: data is encrypted, but no password is given, exiting...\n");
            else
                mexErrMsgTxt("Error: wrong password for encrypted data, exiting...\n");
        }
    }
    
    // check if a session-struct should be returned
    if (nlhs > 0) {
        
        // map session object to matlab output struct
        // and assign to the first return argument
        plhs[0] = map_mef3_session(session, map_indices_flag);
        
    }
    
    // free the session memory
    free_session(session, MEF_TRUE);
    
    //
    return;
    
}

// [EOF]
