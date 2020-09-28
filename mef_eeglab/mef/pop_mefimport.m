function [EEG, com] = pop_mefimport(EEG, varargin)
% POP_MEFIMPORT Import MEF data into EEGLab with/out GUI
%
% Syntax:
%   [EEG, com] = pop_mefimport(EEG)
%   [EEG, com] = __(__, mef_ver)
%   [EEG, com] = __(__, sess_path)
%   [EEG, com] = __(__, sess_path, sel_chan)
%   [EEG, com] = __(__, sess_path, sel_chan, start_end)
%   [EEG, com] = __(__, sess_path, sel_chan, start_end, unit)
%   [EEG, com] = __(__, pw)
%
% Input(s):
%   EEG             - [strcut] EEGLab dataset structure. See Note for
%                     addtional information about the details of the
%                     structure.
%   sess_path       - [str] path of the session
%   sel_chan        - [string array] the name(s) of the data files in the
%                     directory of sess_path.
%   start_end       - [1 x 2 array] (optional) relative [start time/index,
%                     end time/index] of the signal to be extracted from
%                     the file (default: the entire signal)
%   unit            - [str] (optional) unit of start_end: 'uUTC' (default), 'Index',
%                     'Second', 'Minute', 'Hour', and 'Day'
%   pw              - [strct] (opt) password structure depending on MEF
%                     version
% 
% Outputs:
%   EEG             - [struct] EEGLab dataset structure. See Note for
%                     addtional information about the details of the
%                     structure.
%   com             - [str] the command output
% 
% Note:
%   All MEF files in one directory are assumed to be data files for
%   different channels during recording.
% 
%   Details of EEG dataset structure in EEGLab can be found at:
%   https://sccn.ucsd.edu/wiki/A05:_Data_Structures, or see the help
%   information of eeg_checkset.m.
%
%   The command output is a hidden output that does not have to be
%   described in the header.
% 
%   start_end is a relative time pint, which is relative to the beginning
%   of data recording.
% 
% See also EEGLAB, mefimport.

% Copyright 2019-2020 Richard J. Cui. Created: Tue 05/07/2019 10:33:48.169 PM
% $Revision: 1.9 $  $Date: Mon 09/28/2020  3:06:06.030 PM $
%
% Rocky Creek Dr NE
% Rochester, MN 55906, USA
%
% Email: richard.cui@utoronto.ca

% =========================================================================
% parse inputs
% =========================================================================
% display help if not enough arguments
% ------------------------------------
if nargin == 0
    com = '';
	help mfilename
	return
end % if	

q = parseInputs(EEG, varargin{:});
EEG = q.EEG;
sess_path = q.sess_path;
sel_chan = q.sel_chan;
start_end = q.start_end;
unit = q.unit;
pw = q.pw;

mef_ver = q.mef_ver;
switch mef_ver
    case 2.1
        mef_eeglab = @MEFEEGLab_2p1;
    case 3.0
        mef_eeglab = @MEFEEGLab_3p0;
    otherwise
        error('pop_mefimport:invalidMEFVersion', 'MEF version is invalid')
end % switch

% =========================================================================
% main
% =========================================================================
% import signal
% --------------
if isempty(sess_path)
    % use GUI to get the necessary information
    this = gui_mefimport(mef_ver); % this - MEFEEGLab_2p1 object
    if isempty(this)
        EEG = [];
        return
    else
        sess_path = this.SessionPath;
        sel_chan = this.SelectedChannel;
        % if GUI is cancelled
        if isempty(sess_path) && isempty(sel_chan)
            EEG = [];
            return
        end % if
    end % if
else
    this = mef_eeglab(sess_path, pw);
    if isempty(sel_chan)
        sel_chan = this.ChannelName;
    end % if
    this.SelectedChannel = sel_chan;
    if isempty(start_end)
        % TODO: how to use proper unit
        unit = this.Unit; % now just ignore uer's unit
        start_end = this.abs2relativeTimePoint(this.BeginStop, unit);
        % start_end_sys = this.abs2relativeTimePoint(this.BeginStop, this.Unit);
        % start_end = this.SessionUnitConvert(start_end_sys, this.Unit, unit);
    end % if
    this.SEUnit = unit;
    this.StartEnd = start_end; % relative time points
end % if
EEG = this.mefimport(EEG);
EEG = eeg_checkset(EEG); % from eeglab functions

% process discontinuity events
% ----------------------------
if height(this.Continuity) > 1
    discont_event = this.findDiscontEvent;
    EEG.event = discont_event;
    EEG.urevent = rmfield(discont_event, 'urevent');
end % if

% keep some data in eeglab space
% ------------------------------
mef_data.this = this; % an instance of MEFEEGLab object
mef_data.start_end = this.StartEnd;
mef_data.unit = this.SEUnit;
EEG.etc.mef_data = mef_data;

% return the string command
% -------------------------
com = sprintf('%s(EEG, [sess_path, [sel_chan, [start_end, [unit, [password]]]]] )', mfilename);

end % funciton

% =========================================================================
% subroutines
% =========================================================================
function q = parseInputs(EEG, varargin)

% defaults
default_mv = 2.1; % mef version
defaultFP = '';
defaultFN = '';
defaultSE = [];
defaultUnit = 'uutc';
expectedUnit = {'index', 'uutc', 'second', 'minute', 'hour', 'day'};
default_pw = struct([]);

valid_se = @(x) isempty(x) || (isnumeric(x) && numel(x) == 2 && x(1) <= x(2));

% parse rules
p = inputParser;
p.addRequired('EEG', @(x) isempty(x) || isstruct(x));
p.addOptional('mef_ver', default_mv, @isnumeric);
p.addOptional('sess_path', defaultFP, @ischar);
p.addOptional('sel_chan', defaultFN, @(x) isempty(x) || ischar(x)...
    || iscellstr(x) || isstring(x));
p.addOptional('start_end', defaultSE, valid_se);
p.addOptional('unit', defaultUnit,...
    @(x) any(validatestring(x, expectedUnit)));
p.addOptional('pw', default_pw, @isstruct);

% parse and return the results
p.parse(EEG, varargin{:});
q = p.Results;

end % function

% [EOF]

