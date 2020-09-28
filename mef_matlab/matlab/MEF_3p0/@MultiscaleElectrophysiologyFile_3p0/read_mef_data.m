function data = read_mef_data(this, channel_path, varargin)
% MultiscaleElectrophysiologyFile_3p0.READ_MEF_DATA Read the MEF 3.0 data from a time-series channel
%	
% Syntax:
%   data = read_mef_data(this, channel_path)
%   data = read_mef_data(__, password)
%   data = read_mef_data(__, password, range_type)
%   data = read_mef_data(__, password, range_type, begin, stop)
% 
% Input(s):
%   this            - [obj] MultiscaleElectrophysiologyFile_3p0 object
%   channel_path    - [char] (opt) path (absolute or relative) to the MEF3 
%                     channel folder
%   password        - [str] (opt) password to the MEF 3.0 data; Pass 
%                     empty string if not encrypted (default = '')
%                     .
%   range_type      - [char] (opt) modality that is used to define the 
%                     data-range to read, either 'time' in uUTC or 'samples'
%                     (default = samples)
%   begin           - [array] (opt) Start-point for the reading of data 
%                     (either as a timepoint or samplenumber); Pass -1 to
%                     start at the first sample of the timeseries. if
%                     range_type is 'smaples', the first sample begins at 1
%                     using Matlab convention. (default = -1)
%   stop            - [array] (opt) End-point to stop the reading data
%                     (either as a timepoint or samplenumber); Pass -1 as
%                     value to end at the last sample of the timeseries
%                     (default = -1)
%
% Output(s): 
%   data            - [array] channel data
%
% Note:
%   When the 'range_type' is set to 'samples', the function returns the
%   sampled data in sequence; if 'range_type' is set to 'time'in uUTC, the
%   function returns the data with NaN representing the missing samples.
%
% See also .

% Richard J. Cui. Adapted: Fri 01/31/2020 11:59:20.073 PM
% $Revision: 0.5 $  $Date: Fri 09/25/2020  1:02:41.636 PM $
%
% Rocky Creek Dr NE
% Rochester, MN 55906, USA
%
% Email: richard.cui@utoronto.ca

% =========================================================================
% parse inputs
% =========================================================================
q = parseInputs(this, channel_path, varargin{:});
ch_path = q.channel_path;
pw = q.password;
rtype = q.range_type;
begin = q.begin;
stop = q.stop;

if isempty(ch_path)
    ch_path = fullfile(this.FilePath, this.FileName);
end % if

if isempty(pw)
    pw = this.processPassword;
end % if

if strcmpi(rtype, 'samples') == true && begin ~= -1
    begin = begin-1; % change to python convention
end % if

% =========================================================================
% main
% =========================================================================
data = decompress_mef_3p0(ch_path, pw, rtype, begin, stop); % mex

end

% =========================================================================
% subroutines
% =========================================================================
function q = parseInputs(varargin)

% defaults
default_cp = '';
default_pw = ''; % password
default_rt = 'samples'; % range_type
default_bg = -1; % begin
default_sp = -1; % stop

expected_type = {'samples', 'time'};

% parse rules
p = inputParser;
p.addRequired('this', @isobject);
p.addOptional('channel_path', default_cp, @isstr);
p.addOptional('password', default_pw, @isstr);
p.addOptional('range_type', default_rt, @(x) any(validatestring(x, expected_type)));
p.addOptional('begin', default_bg, @isnumeric);
p.addOptional('stop', default_sp, @isnumeric);

% parse and return the results
p.parse(varargin{:});
q = p.Results;

end % funciton

% [EOF]