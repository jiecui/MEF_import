function maf_fname = gui_mafimport()
% GUI_MAFIMPORT Graphic UI for importing MAF event information

[file, path] = uigetfile('*.maf', 'Select a MAF file');

if file
    maf_fname = fullfile(path, file);
else
    maf_fname = '';
end % if

end % function

% [EOF]