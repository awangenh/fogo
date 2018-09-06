

function dirList = constructDirList

% The list of dirs (unfortunatelly, this cell array has to be constructed
%by hand) 
dirList = cell(8, 2);
%Image Number
dirList{1,1} = '3096';
dirList{2,1} = '22090';
dirList{3,1} = '48055';
dirList{4,1} = '118035';
dirList{5,1} = '124084';
dirList{6,1} = '138078';
dirList{7,1} = '143090';
dirList{8,1} = '219090';
%Evaluator number
dirList{1,2} = '1123'; dirList{1,3} = '1105'; dirList{1,4} = '1107'; dirList{1,5} = '1121';
dirList{2,2} = '1106'; dirList{2,3} = '1107'; dirList{2,4} = '1108'; dirList{2,5} = '1109';  dirList{2,6} = '1115';  dirList{2,7} = '1121';
dirList{3,2} = '1105'; dirList{3,3} = '1115'; dirList{3,4} = '1119'; dirList{3,5} = '1121';  dirList{3,6} = '1130';
dirList{4,2} = '1105'; dirList{4,3} = '1108'; dirList{4,4} = '1115'; dirList{4,5} = '1117';  dirList{4,6} = '1130';
dirList{5,2} = '1103'; dirList{5,3} = '1107'; dirList{5,4} = '1115'; dirList{5,5} = '1116';  dirList{5,6} = '1123';
dirList{6,2} = '1105'; dirList{6,3} = '1107'; dirList{6,4} = '1108'; dirList{6,5} = '1112';  dirList{6,6} = '1116';  dirList{6,7} = '1124';
dirList{7,2} = '1105'; dirList{7,3} = '1109'; dirList{7,4} = '1115'; dirList{7,5} = '1121';  dirList{7,6} = '1124';
dirList{8,2} = '1103'; dirList{8,3} = '1107'; dirList{8,4} = '1113'; dirList{8,5} = '1115';  dirList{8,6} = '1127';