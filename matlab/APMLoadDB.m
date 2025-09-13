function APMBinaryLog
    % Use clc directly, no need for eval
    clc;
    disp('clear all');
    disp('waiting ...');

    [filename, pathname] = uigetfile({'*.db';'*.bin';'*.*'}, 'Open log file');
    
    % --- CHANGE: Robustly check for uigetfile cancellation ---
    if isequal(filename, 0) || isequal(pathname, 0)
        disp('User cancelled operation.');
        return; 
    end
    
    % --- CHANGE: Use fullfile for robust path creation ---
    fullname = fullfile(pathname, filename);

    split_file_name = regexp(filename, '\.', 'split');

    if strcmp(split_file_name{end}, 'db') % More robust way to check extension
        disp('Reading .db file...');
        conn = sqlite(fullname, 'readonly');
        group_name = fetch(conn,'SELECT name FROM maintable');
        labels = fetch(conn,'SELECT labels FROM maintable');
        names={};
        tic;

        for i = 1:height(group_name)
            current_group_name = group_name.name(i);
            
            sql_query = "SELECT COUNT(*) FROM " + current_group_name;
            count_result = fetch(conn, sql_query);
            valid_group = count_result{1, 1};
        
            if valid_group == 0
                continue;
            else
                % This part is already modernized and correct
                sql_select_all_query = "SELECT * FROM " + current_group_name;
                content = fetch(conn, sql_select_all_query);
                
                C={}; FIELDS={};
                colums = width(content); % Use width() for tables

                for j = 1:colums
                    % --- CHANGE: Correct indexing to extract data from the table column ---
                    column_data = content{:, j}; 
                    if isnumeric(column_data)
                        C{j} = double(column_data);
                    else
                        C{j} = column_data;
                    end
                end

                % --- CHANGE: Correct indexing on 'labels' table ---
                FIELDS = regexp(labels{i, 1}, ',', 'split');
                
                % This logic assumes the first column is always dropped
                C = C(1, 2:end);
                
                % --- CHANGE: Create struct and assign to base workspace WITHOUT eval ---
                
                % 1. Get the desired variable name as a string (e.g., 'adsb')
                new_var_name = lower(char(current_group_name));

                % 2. Create the struct with your data
                data_struct = cell2struct(C, FIELDS, 2);
                
                % 3. Assign the struct to a variable with that name in the 'base' workspace
                assignin('base', new_var_name, data_struct);
                
                % --- CHANGE: Correct indexing for 'names' cell array ---
                names{end + 1, 1} = new_var_name;
            end
        end
        close(conn);
        toc;
        assignin('base','names',sort(names));
        clear C colums content group_name i j labels valid_group FIELDS;
        disp('Done.');
    end
end