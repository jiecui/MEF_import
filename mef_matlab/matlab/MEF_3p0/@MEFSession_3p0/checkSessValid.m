function valid_yn = checkSessValid(this, varargin)
% MEFSESSION_3P0.checkSessValid check validation of session data
% 
% Syntax:
%   valid_yn = checkSessValid(this)
%   valid_yn = checkSessValid(this, sess_info)
%   valid_yn = checkSessValid(this, sess_info, password)
% 
% Input(s):
%   this            - [obj] MEFSession_3p0 object
%   sess_info       - [table] (opt) session information table (see
%                     get_sessinfo.m for details)
%   password        - [struct] (opt) password structure (see
%                     MEFSession_3p0.m)
% 
% Output(s):
%   valid_yn        - [logical] whether the session data is validated or
%                     not
% 
% Note:
%   At this point, this routine checks the following properties of MEF 2.1
%   channel data: Sampling frequncy, Begin point, Stop point, Number of
%   samples, Number of index entry, Number of discountinuity entry, Subject
%   encryption, Session encryption, Data encryption, MEF fromat version.
% 
% Example:
% 
% Sess also get_sessinfo, MEFSession_3p0.

% Copyright 2020 Richard J. Cui. Created: Fri 01/03/2020  4:19:10.683 PM
% $ Revision: 0.5 $  $ Date: Mon 11/02/2020 10:14:57.073 AM $
%
% Rocky Creek Dr NE
% Rochester, MN 55906, USA
%
% Email: richard.cui@utoronto.ca

% =========================================================================
% Main process
% =========================================================================
% parse inputs
% -------------
q = parse_input(this, varargin{:});
sess_info = q.sess_info;
pw = q.password;

if isempty(sess_info)
    sess_info = this.SessionInformation;
end % if
if isempty(pw)
    pw = this.Password;
end % if

% validate the data
% -----------------
if isempty(sess_info)
    valid_yn = false;
else
    warning('off','backtrace'); 
    % check sampling frequency
    fs_yn = check_samp_freq(sess_info);
    % check begin point
    bg_yn = check_begin(sess_info);
    % check stop point
    sp_yn = check_stop(sess_info);
    % check sample numbers
    sm_yn = check_samples(sess_info);
    % check index entry
    ie_yn = check_indexentry(sess_info);
    % check discountinuity entry
    de_yn = check_discentry(sess_info);
    % check subject password
    sj_yn = check_subjencry(sess_info, pw);
    % check session password
    sn_yn = check_sessencry(sess_info, pw);
    % check data password
    dt_yn = check_dataencry(sess_info, pw);
    % check version
    vr_yn = check_ver(sess_info);
    % check institution
    in_yn = check_inst(sess_info);
    % check subject id
    sb_yn = check_subjid(sess_info);
    % check acquisition system
    ac_yn = check_acqsys(sess_info);
    % check compression algorithm
    cs_yn = check_compalg(sess_info);
    
    valid_yn = fs_yn & bg_yn & sp_yn & sm_yn & ie_yn & de_yn & sj_yn ...
               & sn_yn & dt_yn & vr_yn & in_yn & sb_yn & ac_yn & cs_yn;
    warning('on')
end % if

end

% =========================================================================
% Subroutines
% =========================================================================
function u_yn = checkUnique(x)
% check uniqueness of an array

if numel(unique(x)) == 1
    u_yn = true;
else
    u_yn = false;
end % if

end % function

function fs_yn = check_samp_freq(sess_info)
% check sampling frequency

fs = sess_info.SamplingFreq;

% check consistency
u_yn = checkUnique(fs);

if u_yn == false
    warning('MEFSession_3p0:checkSessValid',...
        'Sampling frequencies of different channels are not consistent')
end % if

% check positivity
if any(fs <= 0)
    p_yn = false;
    warning('MEFSession_3p0:checkSessValid',...
        'Sampling frequencies in some channels may not be valid')    
else
    p_yn = true;
end % if

fs_yn = u_yn & p_yn;

end % function

function bg_yn = check_begin(sess_info)
% check begin point of data

begin = sess_info.Begin;

% check consistency
u_yn = checkUnique(begin);

if u_yn == false
    warning('MEFSession_3p0:checkSessValid',...
        'Begin points of differenct channels are not consistent')
end % if

% check positivity
if any(begin < 0)
    p_yn = false;
    warning('MEFSession_3p0:checkSessValid',...
        'Begin points in some channels may not be valid')    
else
    p_yn = true;
end % if

bg_yn = u_yn & p_yn;

end % function

function sp_yn = check_stop(sess_info)
% check stop point of data

stop = sess_info.Stop;

% check consistency
u_yn = checkUnique(stop);

if u_yn == false
    warning('MEFSession_3p0:checkSessValid',...
        'Stop points of differenct channels are not consistent')
end % if

% check positivity
if any(stop <= 0)
    p_yn = false;
    warning('MEFSession_3p0:checkSessValid',...
        'Stop points in some channels may not be valid')    
else
    p_yn = true;
end % if

sp_yn = u_yn & p_yn;

end % function

function sm_yn = check_samples(sess_info)
% check number of sample of data

samples = sess_info.Samples;
% check consistency
u_yn = checkUnique(samples);

if u_yn == false
    warning('MEFSession_3p0:checkSessValid',...
        'Numbers of samples of differenct channels are not consistent')
end % if

% check positivity
if any(samples < 0)
    p_yn = false;
    warning('MEFSession_3p0:checkSessValid',...
        'Number of samples in some channels may not be valid')    
else
    p_yn = true;
end % if

sm_yn = u_yn * p_yn;

end % function

function ie_yn = check_indexentry(sess_info)
% check number of index entry of data

ind = sess_info.IndexEntry;
% check consistency
u_yn = checkUnique(ind);

if u_yn == false
    warning('MEFSession_3p0:checkSessValid',...
        'Numbers of index entry of differenct channels are not consistent')
end % if

% check positivity
if any(ind <= 0)
    p_yn = false;
    warning('MEFSession_3p0:checkSessValid',...
        'Number of index entry in some channels may not be valid')    
else
    p_yn = true;
end % if

ie_yn = u_yn & p_yn;

end % function

function de_yn = check_discentry(sess_info)
% check number of discountiunity entry of data

disc = sess_info.DiscountinuityEntry;

% check consistency
u_yn = checkUnique(disc);

if u_yn == false
    warning('MEFSession_3p0:checkSessValid',...
        'Numbers of discountinuity entry of differenct channels are not consistent')
end % if

% check positivity
if any(disc <= 0)
    p_yn = false;
    warning('MEFSession_3p0:checkSessValid',...
        'Numbers of discountinuity entry in some channels may not be valid')    
else
    p_yn = true;
end % if

de_yn = u_yn & p_yn;

end % function

function sj_yn = check_subjencry(sess_info, pw)
% TODO: check requirement of subject encryption 

sj_yn = true;

return

% check consistency
cy_yn = checkUnique(sess_info.SubjectEncryption);
pw_yn = false;

if cy_yn == false
    warning('MEFSession_3p0:checkSessValid',...
        'Requirements of subject encryption of differenct channels are not consistent')
else
    % check password avilability
    subj_encry = unique(sess_info.SubjectEncryption);
    pw_yn = ~(subj_encry & isempty(pw.Subject));
    
    if pw_yn == false
        warning('MEFSession_3p0:checkSessValid',...
            'Subject password is required, but may not be provided')
    end % if
end % if

sj_yn = cy_yn & pw_yn;

end % function

function sn_yn = check_sessencry(sess_info, pw)
% TODO: check requirement of session encryption 

sn_yn = true;
return

% check consistency
cy_yn = checkUnique(sess_info.SessionEncryption);
pw_yn = false;

if cy_yn == false
    warning('MEFSession_3p0:checkSessValid',...
        'Requirements of session encryption of differenct channels are not consistent')
else
    % check password avilability
    sess_encry = unique(sess_info.SessionEncryption);
    pw_yn = ~(sess_encry & isempty(pw.Session));
    
    if pw_yn == false
        warning('MEFSession_3p0:checkSessValid',...
            'Session password is required, but may not be provided')
    end % if
end % if

sn_yn = cy_yn & pw_yn;

end % function

function dt_yn = check_dataencry(sess_info, pw)
% TODO: check requirement of data encryption 

dt_yn = true;
return

% check consistency
cy_yn = checkUnique(sess_info.DataEncryption);
pw_yn = false;

if cy_yn == false
    warning('MEFSession_3p0:checkSessValid',...
        'Requirements of data encryption of differenct channels are not consistent')
else
    % check password avilability
    data_encry = unique(sess_info.DataEncryption);
    pw_yn = ~(data_encry & isempty(pw.Data));
    
    if pw_yn == false
        warning('MEFSession_3p0:checkSessValid',...
            'Data password is required, but may not be provided')
    end % if
end % if

dt_yn = cy_yn & pw_yn;

end % function

function vr_yn = check_ver(sess_info)
% check version consistency

% check consistency
cy_yn = checkUnique(sess_info.Version);

if cy_yn == false
    warning('MEFSession_3p0:checkSessValid',...
        'MEF data versions of differenct channels are not consistent')
else
    % check version number
    ver = unique(sess_info.Version);
    vrs_yn = ver == '3.0';
    if vrs_yn == false
        warning('MEFSession_3p0:checkSessValid',...
            'The MEF channel is in format version %s, rather than 2.1. The results may be unpredictable.',...
            sess_info.Version(1))
    end % fi
end % if

vr_yn = cy_yn & vrs_yn;

end % function

function in_yn = check_inst(sess_info)
% check institution

% check consistency
in_yn = checkUnique(sess_info.Institution);

if in_yn == false
    warning('MEFSession_3p0:checkSessValid',...
        'Institution names of differenct channels are not consistent')
end % if

end % function

function sb_yn = check_subjid(sess_info)
% check institution

% check consistency
sb_yn = checkUnique(sess_info.Institution);

if sb_yn == false
    warning('MEFSession_3p0:checkSessValid',...
        'Subject IDs of differenct channels are not consistent')
end % if

end % function

function ac_yn = check_acqsys(sess_info)
% check acquisition system

% check consistency
ac_yn = checkUnique(sess_info.AcquisitionSystem);

if ac_yn == false
    warning('MEFSession_3p0:checkSessValid',...
        'Acquisiton systems of differenct channels are not consistent')
end % if

end % function

function cs_yn = check_compalg(sess_info)
% check compression algorithm

% check consistency
cs_yn = checkUnique(sess_info.CompressionAlgorithm);

if cs_yn == false
    warning('MEFSession_3p0:checkSessValid',...
        'Compression algorithms of differenct channels are not consistent')
end % if

end % function

function q = parse_input(this, varargin)

% defaults
default_si = table; % default of session information table
default_pw = struct([]); % default of password

% parse inputs
p = inputParser;
p.addRequired('this', @isobject);
p.addOptional('sess_info', default_si, @istable);
p.addOptional('password', default_pw, @isstruct);

% parse and return the results
p.parse(this, varargin{:});
q = p.Results;

end % function

% [EOF]