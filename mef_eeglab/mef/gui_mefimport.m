function varargout = gui_mefimport(varargin)
% GUI_MEFIMPORT Graphic UI for importing MEF 2.1 datafile
% 
% Syntax:
%   this = gui_mefimport()
% 
% Input(s):
% 
% Output(s):
%   this            - [obj] MEFEEGLab_2p1 object
% 
% Note:
%   gui_mefimport does not import MEF by itself, but instead gets the
%   necessary information about the data and then relys on gui_mefimport.m to
%   import MEF data into EEGLab.
% 
% See also pop_mefimport, gui_mefimport.

% Copyright 2019-2020 Richard J. Cui. Created: Sun 04/28/2019  9:51:01.691 PM
% $Revision: 1.4 $  $Date: Mon 09/28/2020  3:06:06.030 PM $
%
% Rocky Creek Dr NE
% Rochester, MN 55906, USA
%
% Email: richard.cui@utoronto.ca

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @gui_mefimport_OpeningFcn, ...
                   'gui_OutputFcn',  @gui_mefimport_OutputFcn, ...
                   'gui_LayoutFcn',  [] , ...
                   'gui_Callback',   []);
if nargin && ischar(varargin{1})
    gui_State.gui_Callback = str2func(varargin{1});
end

if nargout
    [varargout{1:nargout}] = gui_mainfcn(gui_State, varargin{:});
else
    gui_mainfcn(gui_State, varargin{:});
end
% End initialization code - DO NOT EDIT

function gui_mefimport_OpeningFcn(hObject, eventdata, handles, varargin)

% parse inputs
% -------------
if isempty(varargin)
    mef_ver = 2.1;
else
    mef_ver = varargin{1};
    if ~any([2.1, 3.0] == mef_ver)
        error('gui_mefimport:invalidMEFVersion', 'the version of MEF is invalid')
    end % if
end % if

% initialization
% --------------
set(handles.checkbox_segment, 'Value', 0)
set(handles.pushbutton_deselall, 'Enable', 'off')
set(handles.uitable_channel,'Data', [], 'Enable', 'off')
set(handles.popupmenu_unit, 'String', {'Index', 'uUTC', 'mSec', 'Second',...
    'Hour', 'Day'}, 'Enable', 'Off')
set(handles.edit_start, 'Enable', 'Off')
set(handles.edit_end, 'Enable', 'Off')
set(handles.uitable_channel, 'Enable' , 'Off')
set(handles.checkbox_segment, 'Enable', 'Off')
set(handles.uipanel_mefimport, 'Title', sprintf('Import MEF %.1f Data', mef_ver))

handles.mef_ver = mef_ver;
handles.old_unit = 'uUTC';

switch mef_ver
    case 2.1
        pw = struct('Subject', '', 'Session', '', 'Data', '');
    case 3.0
        pw = struct('Level1Password', '', 'Level2Password', '',...
            'AccessLevel', 0);
end % switch
handles.pw = pw;

handles.output = hObject;
guidata(hObject, handles);

uiwait();

function varargout = gui_mefimport_OutputFcn(hObject, eventdata, handles)
if isempty(handles) || isfield(handles, 'this') == false
    varargout{1} = [];
else
    varargout{1} = handles.this;
end % if
guimef = findobj('Tag', 'gui_mefimport');
delete(guimef)

function [start_end, unit] = getStartend(this, handles)
% get start and end point

unit_list = get(handles.popupmenu_unit, 'String');
choice = get(handles.popupmenu_unit, 'Value');
unit = unit_list{choice};

start_end_uutc = this.abs2relativeTimePoint(this.BeginStop, this.Unit);
start_end = this.SessionUnitConvert(start_end_uutc, this.Unit, unit);


function pushbutton_folder_Callback(hObject, eventdata, handles)
% get data folder and obtain corresponding data information

% get data folder
% ---------------
sess_path= uigetdir;
if sess_path == 0
    return
else
    set(handles.edit_path, 'String', sess_path);
end

% get data info
% -------------
mef_ver = handles.mef_ver;
switch mef_ver
    case 2.1
        mef_eeglab = @MEFEEGLab_2p1;
    case 3.0
        mef_eeglab = @MEFEEGLab_3p0;
end % switch

pw = handles.pw;
this = mef_eeglab(sess_path, pw);

% get start and end points of imported signal in sample index
[start_end, unit] = getStartend(this, handles);
this.StartEnd = start_end;
this.SEUnit = unit;

set(handles.edit_start, 'String', num2str(start_end(1)))
set(handles.edit_end, 'String', num2str(start_end(2)))
handles.start_end = start_end;
handles.old_unit = unit;
handles.unit = unit;

% get channel information
channame = this.ChannelName(:);
num_chan = numel(channame);
Table = cell(num_chan, 2);
Table(:, 1) = convertStringsToChars(channame);
Table(:, 2) = num2cell(true(num_chan, 1));
rownames = num2cell(num2str((1:num_chan)'));

handles.list_chan = this.ChannelName;
this.SelectedChannel = handles.list_chan;
handles.this = this;
guidata(hObject, handles)

% display import information
set(handles.uitable_channel, 'Data', Table, 'RowName', rownames, 'Enable' , 'On')
set(handles.pushbutton_deselall, 'Enable', 'On')
set(handles.checkbox_segment, 'Enable', 'On')
set(handles.popupmenu_unit, 'Enable', 'On')

% display session information
set(handles.text_subjid_val, 'String', this.SubjectID)
set(handles.text_institute_val, 'String', this.Institution)
set(handles.text_sampfreq_val, 'String', num2str(this.SamplingFrequency))
set(handles.text_samples_val, 'String', num2str(this.Samples))
set(handles.text_datablk_val, 'String', num2str(this.DataBlocks))
set(handles.text_timegaps_val, 'String', num2str(this.TimeGaps))


function edit_path_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function edit_path_Callback(hObject, eventdata, handles)


function checkbox_segment_Callback(hObject, eventdata, handles)

if get(handles.checkbox_segment, 'Value') == true
    set(handles.edit_start, 'Enable', 'On')
    set(handles.edit_end, 'Enable', 'On')
elseif get(handles.checkbox_segment, 'Value') == false
    set(handles.edit_start, 'Enable', 'Off')
    set(handles.edit_end, 'Enable', 'Off')
end % if

function SelectedCells= uitable_channel_CellSelectionCallback(hObject, eventdata, handles)
SelectedCells = eventdata.Indices;


function pushbutton_ok_Callback(hObject, eventdata, handles)
% get the data file information

if isfield(handles, 'edit_path') && isfield(handles, 'list_chan')...
        && ~isempty(handles.edit_path) && ~isempty(handles.list_chan)
    % get MEFEEGLab_2p1 object
    this = handles.this;
    
    % sess_path
    handles.sess_path = get(handles.edit_path, 'String');
    this.SessionPath = handles.sess_path;
    
    % sel_chan
    Table = get(handles.uitable_channel, 'Data');
    list_chan = handles.list_chan;
    choice = cell2mat(Table(:, end));
    handles.sel_chan = list_chan(choice);
    this.SelectedChannel = handles.sel_chan;
    
    % unit
    unit_list = get(handles.popupmenu_unit, 'String');
    choice = get(handles.popupmenu_unit, 'Value');
    handles.unit = unit_list{choice};
    this.SEUnit = handles.unit;
    
    % start_end
    this = handles.this;    
    start_pt = str2double(get(handles.edit_start, 'String'));
    end_pt = str2double(get(handles.edit_end, 'String'));
    handles.start_end = [start_pt, end_pt];
    this.StartEnd = handles.start_end;
    
    handles.this = this;
    
    guidata(hObject, handles);
    
    % close the GUI
    uiresume();
    guimef= findobj('Tag', 'gui_mefimport');
    close(guimef)
else
    supergui( 'geomhoriz', { 1 1 1 }, 'uilist', { ...
        { 'style', 'text', 'string', 'No valid MEF file!',...
                'HorizontalAlignment', 'center' },...
        { }, ...
        { 'style', 'pushbutton' , 'string', 'OK', 'callback',...
                'close(gcbf);' } } );
end % if


function pushbutton_cancel_Callback(hObject, eventdata, handles)

handles.sess_path = '';
handles.sel_chan = '';
handles.start_end = [];
handles.unit = '';
handles.this = [];
guidata(hObject, handles)

uiresume();
guimef= findobj('Tag', 'gui_mefimport');
close(guimef)


function pushbutton_deselall_Callback(hObject, eventdata, handles)
Table = get(handles.uitable_channel, 'Data');
r = size(Table, 1);
for i = 1:r
    Table{i, end} = false;
end

set(handles.uitable_channel, 'Data', Table)
set(handles.uitable_channel, 'Enable' , 'On')

function gui_mefimport_CloseRequestFcn(hObject, eventdata, handles)

if ~isfield(handles, 'sess_path')
    handles.sess_path = '';
end % if

if ~isfield(handles, 'sel_chan')
    handles.sel_chan = '';
end % if

if ~isfield(handles, 'start_end')
    handles.start_end = [];
end % if

if ~isfield(handles, 'unit')
    handles.unit = '';
end % if
guidata(hObject, handles)

uiresume();


function edit_start_Callback(hObject, eventdata, handles)
% hObject    handle to edit_start (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit_start as text
%        str2double(get(hObject,'String')) returns contents of edit_start as a double


% --- Executes during object creation, after setting all properties.
function edit_start_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit_start (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function edit_end_Callback(hObject, eventdata, handles)
% hObject    handle to edit_end (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit_end as text
%        str2double(get(hObject,'String')) returns contents of edit_end as a double


% --- Executes during object creation, after setting all properties.
function edit_end_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit_end (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on selection change in popupmenu_unit.
function popupmenu_unit_Callback(hObject, eventdata, handles)
% hObject    handle to popupmenu_unit (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: contents = cellstr(get(hObject,'String')) returns popupmenu_unit contents as cell array
%        contents{get(hObject,'Value')} returns selected item from popupmenu_unit

% unit
unit_list = get(handles.popupmenu_unit, 'String');
choice = get(handles.popupmenu_unit, 'Value');
unit = unit_list{choice};
old_unit = handles.old_unit;

if strcmpi(unit, old_unit) == true
    return
end % if

% MEFEEGLab object
this = handles.this;

% change value according to the unit chosen
old_start = str2double(get(handles.edit_start, 'String'));
old_end = str2double(get(handles.edit_end, 'String'));
new_start = this.SessionUnitConvert(old_start, old_unit, unit);
new_end = this.SessionUnitConvert(old_end, old_unit, unit);

set(handles.edit_start, 'String', num2str(new_start, 32));
set(handles.edit_end, 'String', num2str(new_end, 32));
handles.start_end = [new_start, new_end];
handles.old_unit = unit;
guidata(hObject, handles)



% --- Executes during object creation, after setting all properties.
function popupmenu_unit_CreateFcn(hObject, eventdata, handles)
% hObject    handle to popupmenu_unit (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: listbox controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in pushbutton_setpasswords.
function pushbutton_setpasswords_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton_setpasswords (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

mef_ver = handles.mef_ver;
switch mef_ver
    case 2.1
        handles = set_pw_mef_2p1(handles);
    case 3.0
        handles = set_pw_mef_3p0(handles);
end % switch

guidata(hObject, handles)

% =========================================================================
% subroutines
% =========================================================================
function handles = set_pw_mef_2p1(handles)

geometry = {[0.5, 1.27], [0.5, 1.27], [0.5, 1.27]};
uilist = {...
    {'style', 'text', 'string', 'Subject', 'fontweight', 'bold'},...
    {'style', 'edit', 'string', '', 'horizontalalignment', 'left',...
        'tooltipstring', 'Input subject password'},...
    {'style', 'text', 'string', 'Session', 'fontweight', 'bold'},...
    {'style', 'edit', 'string', '', 'horizontalalignment', 'left',...
        'tooltipstring', 'Input Session password'},...
    {'style', 'text', 'string', 'Data', 'fontweight', 'bold'},...
    {'style', 'edit', 'string', '', 'horizontalalignment', 'left',...
        'tooltipstring', 'Input Data password'},...    
    };

res = inputgui(geometry, uilist, 'pophelp(''pop_mefimport'')', ...
    'Set MEF passwords -- gui_mefimport()');

if ~isempty(res)
    handles.pw.Subject = res{1};
    handles.pw.Session = res{2};
    handles.pw.Data = res{3};
end % if

function handles = set_pw_mef_3p0(handles)

geometry = {[0.5, 1.27], [0.5, 1.27], [0.5, 1.27]};
uilist = {...
    {'style', 'text', 'string', 'Level1Password', 'fontweight', 'bold'},...
    {'style', 'edit', 'string', '', 'horizontalalignment', 'left',...
        'tooltipstring', 'Input level 1 password'},...
    {'style', 'text', 'string', 'Level2Password', 'fontweight', 'bold'},...
    {'style', 'edit', 'string', '', 'horizontalalignment', 'left',...
        'tooltipstring', 'Input level 2 password'},...
    {'style', 'text', 'string', 'AccessLevel', 'fontweight', 'bold'},...
    {'style', 'edit', 'string', '', 'horizontalalignment', 'left',...
        'tooltipstring', 'Input access level'},...    
    };

res = inputgui(geometry, uilist, 'pophelp(''pop_mefimport'')', ...
    'Set MEF passwords -- gui_mefimport()');

if ~isempty(res)
    handles.pw.Level1Password = res{1};
    handles.pw.Level2Password = res{2};
    handles.pw.AccessLevel = str2double(res{3});
end % if


% [EOF]
