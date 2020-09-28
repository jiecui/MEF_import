classdef MEFEEGLab_3p0 < MEFSession_3p0 & MEFEEGLab
    % MEFEEGLAB_3P0 process MEF 3.0 in EEGLab
    % 
    % Syntax:
    %   this = MEFSession_3p0(sesspath)
    %   this = __(__, password)
    %
    % Input(s):
    %   sesspath    - [str] session path
    %   password    - [struct] (opt) password (default: empty)
    %                 .Level1Password
    %                 .Level2Password
    %                 .AccessLevel
    %
    % Output(s):
    %   this        - [obj] MEFEEGLab_3p0 object
    %
    % Note:
    %
    % See also .
    
    % Copyright 2020 Richard J. Cui. Created: Sun 02/09/2020  3:45:09.696 PM
    % $Revision: 0.4 $  $Date: Mon 09/28/2020  3:06:06.030 PM $
    %
    % Rocky Creek Dr NE
    % Rochester, MN 55906, USA
    %
    % Email: richard.cui@utoronto.ca
    
    % =====================================================================
    % properties
    % =====================================================================
    properties

    end
    
    % =====================================================================
    % methods
    % =====================================================================
    % the constructor
    % ----------------
    methods
        function this = MEFEEGLab_3p0(varargin)
            % class constructor
            % =================
            % parse inputs
            % ------------
            % default
            default_pw = struct('Level1Password', '',...
                'Level2Password', '', 'AccessLevel', 1);
            
            % parse rules
            p = inputParser;
            p.addRequired('sesspath', @isstr);
            p.addOptional('password', default_pw, @isstruct);
            
            % parse the return the results
            p.parse(varargin{:});
            q = p.Results;
            
            % operations during construction
            % ------------------------------
            % call super class
            this@MEFSession_3p0(q.sesspath, q.password);
        end %function
    end % methods
    
    % other methods
    % -------------
    methods
        
    end % methods
end

% [EOF]