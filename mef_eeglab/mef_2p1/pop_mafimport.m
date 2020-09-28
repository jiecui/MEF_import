function [EEG, com] = pop_mafimport(EEG, varargin)
% POP_MAFIMPORT Import MAF event data into EEGLAB with GUI
% 
% Syntax:
%   [EEG, com] = pop_mafimport(EEG)
%   [EEG, com] = pop_mafimport(EEG, wholename)
%   [EEG, com] = pop_mafimport(EEG, wholename, start_end)
%   [EEG, com] = pop_mafimport(EEG, wholename, start_end, unit)
% 
% Input(s):
%   wholename       - [str] full path and name of MAF file
%   start_end       - [1 x 2 array] (optional) [start time/index, end time/index] of 
%                     the signal to be extracted fromt he file (default:
%                     the entire signal)
%   unit            - [str] (optional) unit of start_end: 'Index' (default), 'uUTC',
%                     'Second', 'Minute', 'Hour', and 'Day'
% 
% Outputs:
%   EEG             - [struct] EEGLab dataset structure. See Note for
%                     addtional information about the details of the
%                     structure.
% Note:
%   The command output [com] is a hidden output that does not have to
% be described in the header
% 
% See also EEGLAB, mafimport, pop_mefimport.

% Copyright 2019-2020 Richard J. Cui. Created: Tue 05/28/2019  3:14:55.269 PM
% $Revision: 0.3 $  $Date: Mon 09/28/2020  3:06:06.030 PM$
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
maf_name = q.wholename;
start_end = q.start_end;
unit = q.unit;

mef_data = EEG.etc.mef_data;
if isempty(start_end)
    start_end = mef_data.start_end;
end % if
if isempty(unit)
    unit = mef_data.unit;
end % if

% =========================================================================
% main
% =========================================================================
% import MAF structure
% --------------------
if isempty(maf_name)
    % get the filename of MAF
    maf_name = gui_mafimport;
    % if GUI is cancelled
    if isempty(maf_name)
        EEG = [];
        return
    end % if
end % if
mef1 = mef_data.mef1;
evt_t = mef1.getMAFEvent(maf_name); % event table
mef_data.mef1 = mef1; % update
EEG.etc.mef_data = mef_data;

var_names = evt_t.Properties.VariableNames;
evt_t.Properties.VariableNames = lower(var_names);

% process events from MAF
% -----------------------
% convert event latency in uutc to relative index
if strcmpi(unit, 'index')
    se_ind = start_end;
else
    se_ind = mef1.SampleTime2Index(start_end, unit);
end % if
lat_ind = mef1.SampleTime2Index(evt_t.latency);
idx = lat_ind >= se_ind(1) & lat_ind <= se_ind(2); % only choose those in start_end
evt_t.latency = lat_ind-se_ind(1)+1;
evt_t(~idx, :) = [];

if isempty(evt_t)
    supergui( 'geomhoriz', { 1 1 1 }, 'uilist', { ...
        { 'style', 'text', 'string',...
        'No known event found in the signal under investigation.',...
        'HorizontalAlignment', 'center' },...
        { }, ...
        { 'style', 'pushbutton' , 'string', 'OK', 'callback',...
        'close(gcbf);' } } );
else
    % combine with original event
    ur_evt = EEG.urevent;
    ur_t = struct2table(ur_evt); % convert to table
    new_evt = [evt_t; ur_t];
    
    % sort from small index to large index
    [~, s] = sort(new_evt.latency);
    evt = new_evt(s, :);
    evt.urevent = (1:height(evt))';
    
    % convert to structure
    EEG.event = table2struct(evt);
    EEG.urevent = rmfield(EEG.event, 'urevent');
    fprintf('Events imported:\n')
    disp(evt)
end % if

% return the command string
% -------------------------
com = sprintf('%s(EEG, [wholename, [start_end, [unit]]])', mfilename);

end

% =========================================================================
% subroutines
% =========================================================================
function q = parseInputs(varargin)

% defaults
defaultWN = '';
defaultSE = [];
defaultUnit = '';
expectedUnit = {'index', 'uutc', 'second', 'minute', 'hour', 'day'};

% parse rules
p = inputParser;
p.addRequired('EEG', @(x) isempty(x) || isstruct(x));
p.addOptional('wholename', defaultWN, @ischar);
p.addOptional('start_end', defaultSE,...
    @(x) isnumeric(x) & numel(x) == 2 & x(1) <= x(2));
p.addOptional('unit', defaultUnit,...
    @(x) any(validatestring(x, expectedUnit)));

% parse and return the results
p.parse(varargin{:});
q = p.Results;

end % function

% [EOF]