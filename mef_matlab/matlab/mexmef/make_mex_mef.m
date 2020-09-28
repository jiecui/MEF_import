% Compile mex files required to process MEF files

% Copyright 2019-2020 Richard J. Cui. Created: Wed 05/29/2019  9:49:29.694 PM
% $Revision: 1.2 $  $Date: Fri 09/25/2020  9:41:56.944 AM $
%
% Rocky Creek Dr NE
% Rochester, MN 55906, USA
%
% Email: richard.cui@utoronto.ca

% =========================================================================
% find the directory of make_mex_mef file
% =========================================================================
cur_dir = pwd; % current directory
% go to mex_mef folder
mex_mef = fileparts(mfilename('fullpath')); % directory of file make_mex_mef.m

% =========================================================================
% processing mex for MEF 2.0 file
% =========================================================================

% =========================================================================
% processing mex for MEF 2.1 file
% =========================================================================
% get the directories of mef_2p1
libmef_2p1 = fullfile(fileparts(fileparts(mex_mef)),'libmef','mef_2p1'); % library
mexmef_2p1 = fullfile(mex_mef,'mef_2p1'); % mex

me_cprintf('Keywords','===== Compiling c-mex for MEF 2.1 data =====\n')
fprintf('Building read_mef_info_2p1.mex*\n')
mex('-output','read_mef_info_2p1',['-I' libmef_2p1],...
    fullfile(mexmef_2p1,'read_mef_info_mex_2p1.c'),...
    fullfile(libmef_2p1,'mef_lib_2p1.c'))
movefile('read_mef_info_2p1.mex*',mexmef_2p1)

fprintf('\n')
fprintf('Building decompress_mef_2p1.mex*\n')
mex('-output','decompress_mef_2p1',['-I' libmef_2p1],...
    fullfile(mexmef_2p1,'decompress_mef_mex_2p1.c'),...
    fullfile(libmef_2p1,'mef_lib_2p1.c'))
movefile('decompress_mef_2p1.mex*',mexmef_2p1)

cd(cur_dir)

% =========================================================================
% processing mex for MEF 3.0 file
% =========================================================================
% get the directories of mef_3p0
libmef_3p0 = fullfile(fileparts(fileparts(mex_mef)),'libmef','mef_3p0'); % library
mexmef_3p0 = fullfile(mex_mef,'mef_3p0'); % mex

fprintf('\n')
me_cprintf('Keywords','===== Compiling c-mex for MEF 3.0 data =====\n')
fprintf('Building read_mef_info_3p0.mex*\n')
mex('-output','read_mef_info_3p0',...
    ['-I' libmef_3p0],['-I' mexmef_3p0],...
    fullfile(mexmef_3p0,'read_mef_info_mex_3p0.c'))
movefile('read_mef_info_3p0.mex*',mexmef_3p0)

fprintf('\n')
fprintf('Building decompress_mef_3p0.mex*\n')
mex('-output','decompress_mef_3p0',...
    ['-I' libmef_3p0],['-I' mexmef_3p0],...
    fullfile(mexmef_3p0,'decompress_mef_mex_3p0.c'))
movefile('decompress_mef_3p0.mex*',mexmef_3p0)

cd(cur_dir)

% [EOF]