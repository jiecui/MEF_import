function OUTEEG = mefimport(this, INEEG, varargin)
% MEFEEGLAB.MEFIMPORT Import MEF 2.1 session data into EEG structure
%
% Syntax:
%   OUTEEG = mefimport(this, INEEG)
%   OUTEEG = mefimport(__, start_end)
%   OUTEEG = mefimport(__, start_end, se_unit)
%   OUTEEG = mefimport(__, 'SelectedChannel', sel_chan, 'Password', pw)
%
% Input(s):
%   this            - [obj] MEFEEGLab_2p1 object
%   INEEG           - [struct] EEGLab dataset structure. See Note for
%                     addtional information about the details of the
%                     structure.
%   start_end       - [1 x 2 array] (optional) relative [start time/index, end
%                     time/index] of the signal to be extracted fromt the
%                     file (default: the entire signal)
%   se_unit         - [str] (optional) unit of start_end: 'uUTC',
%                     'Index', 'Second', 'Minute', 'Hour', and 'Day'
%                     (default: '')
%   sel_chan        - [str array] (para) the names of the selected channels
%                     (default: all channels)
%   pw              - [str] (para) passwords of MEF file
%                     .Subject      : subject password (default - '')
%                     .Session
%                     .Data
% 
% Outputs:
%   OUTEEG           - [struct] EEGLab dataset structure. See Note for
%                      addtional information about the details of the
%                      structure.
% 
% Note:
%   Current version assumes continuous signal.
% 
%   All MEF files in one directory are assumed to be data files for
%   different channels during recording.
% 
%   Details of EEG dataset structure in EEGLab can be found at:
%   https://sccn.ucsd.edu/wiki/A05:_Data_Structures, or see the help
%   information of eeg_checkset.m.
%
% See also eeglab, eeg_checkset, pop_mefimport. 

% Copyright 2019-2020 Richard J. Cui. Created: Wed 05/08/2019  3:19:29.986 PM
% $Revision: 2.0 $  $Date: Mon 09/28/2020  3:06:06.030 PM $
%
% Rocky Creek Dr NE
% Rochester, MN 55906, USA
%
% Email: richard.cui@utoronto.ca

% =========================================================================
% parse inputs
% =========================================================================
q = parseInputs(this, INEEG, varargin{:});
INEEG = q.INEEG;
% begin and stop points
start_end = q.start_end;
if isempty(start_end)
    start_end = this.StartEnd;
else
    this.StartEnd = start_end;
end % if
% unit
se_unit = q.se_unit;
if isempty(se_unit)
    se_unit = this.SEUnit;
else
    this.SEUnit = se_unit;
end % if
% selected channel
sel_chan = q.SelectedChannel;
if isempty(sel_chan)
    sel_chan = this.SelectedChannel;
else
    this.SelectedChannel = sel_chan;
end % if
% password
pw = q.Password;
if isempty(pw)
    pw = this.Password;
else
    this.Password = pw;
end % if

sess_path = this.SessionPath;

% =========================================================================
% convert to EEG structure
% =========================================================================
% set EEG structure
% -----------------
if isempty(INEEG)
    % if EEGLAB is included in pathway, this can be done with eeg_emptyset.m
    OUTEEG = struct('setname', '',...
                 'filename', '',...
                 'filepath', '',...
                 'subject','',...
                 'group', '',...
                 'condition', '',...
                 'session', [],...
                 'comments', '',...
                 'nbchan', 0,...
                 'trials',0,...
                 'pnts', 0,...
                 'srate', 1,...
                 'xmin', 0,...
                 'xmax', 0,...
                 'times', [],...
                 'data', [],...
                 'icaact', [],...
                 'icawinv', [],...
                 'icasphere', [],...
                 'icaweights', [],...
                 'icachansind', [],...
                 'chanlocs', [],...
                 'urchanlocs',[],...
                 'chaninfo', [],...
                 'ref', [],...
                 'event', [],...
                 'urevent', [],...
                 'eventdescription', {},...
                 'epoch', [],...
                 'epochdescription', {},...
                 'reject', [],...
                 'stats', [],...
                 'specdata', [],...
                 'specicaact', [],...
                 'splinefile', '',...
                 'icasplinefile', '',...
                 'dipfit', [],...
                 'history', '',...
                 'saved', 'no',...
                 'etc', []);
else
    OUTEEG = INEEG; % careful, if don't clear working space
end % if

% setname
% -------
OUTEEG.setname = sprintf('Data from %s', this.Institution);

% subject
% -------
OUTEEG.subject = this.SubjectID;

% trials
% ------
OUTEEG.trials = 1; % assume continuous recording, not epoched

% nbchan
% ------
% MEF records each channel in different file
OUTEEG.nbchan = numel(sel_chan);

% srate
% -----
OUTEEG.srate = this.SamplingFrequency; % in Hz

% data
% ----
[data, t_index] = this.importSession(start_end, se_unit, sess_path,...
    'SelectedChannel', sel_chan, 'Password', pw);
OUTEEG.data = data;

% xmin, xmax (in second)
% ----------------------
t_sec = this.SessionUnitConvert(t_index, 'index', 'second');
OUTEEG.xmin = min(t_sec);
OUTEEG.xmax = max(t_sec);

% times (in second)
% ------------------------------------------------------------
OUTEEG.times = t_sec; 

% pnts
% ----
OUTEEG.pnts = numel(t_sec);

% comments
% --------
OUTEEG.comments = sprintf('Acauisition system - %s\ncompression algorithm - %s',...
    this.AcquisitionSystem, this.CompressionAlgorithm);

% saved
% -----
OUTEEG.saved = 'no'; % not saved yet

% chanlocs
% --------
chanlocs = struct([]);
sel_chan = this.SelectedChannel;
for k = 1:numel(sel_chan)
    chanlocs(k).labels = convertStringsToChars(sel_chan(k));
end % for
OUTEEG.chanlocs = chanlocs;

end % function

% =========================================================================
% subroutines
% =========================================================================
function q = parseInputs(varargin)

% defaults
default_se = [];
default_ut = '';
expected_ut = {'index', 'uutc', 'msec', 'second', 'minute', 'hour', 'day'};
default_sc = [];
default_pw = struct([]);

% parse rules
p = inputParser;
p.addRequired('this', @isobject);
p.addRequired('INEEG', @(x) isempty(x) || isstruct(x));
p.addOptional('start_end', default_se,...
    @(x) isnumeric(x) & numel(x) == 2 & x(1) <= x(2));
p.addOptional('se_unit', default_ut,...
    @(x) any(validatestring(x, expected_ut)));
p.addParameter('SelectedChannel', default_sc, @isstring) % must be string array
p.addParameter('Password', default_pw, @isstruct);

% parse and return the results
p.parse(varargin{:});
q = p.Results;

end % function

% [EOF]
