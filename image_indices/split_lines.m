%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
% Autor: Fogo
%
% Esta função varre um string e, a cada quebra de linha ou espaço, cria uma
% nova posição no array. O resultado é um array de strings em vez de um
% string único
% 
% OBSERVAÇÃO: o resultado é um array de array de caracteres, em que cada
% linha corresponderá a um string
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function   strlist = split_lines(str)

size_str = length(str);

count_lines = 1;
count_chars = 1;
for i=1:size_str
    if ~(str(i) == ' ')

        if isspace(str(i))
            count_lines = count_lines + 1;
            count_chars = 1;        
        else
            strlist(count_lines,count_chars) = str(i);
            count_chars = count_chars + 1;
        end;
    
    end;
end;

strlist;