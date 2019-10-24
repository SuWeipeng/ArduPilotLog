for i = 1:size(stat.id,1)
    FSM_label{stat.id(i, 1)+1, 1} = sprintf('%s%d','id',stat.id(i, 1));
    FSM(stat.id(i, 1)+1, 1)       = stat.id(i, 1);
end
C = cell(2, size(FSM_label, 1));
for i = 1:1:size(stat.id,1)
    C{1, stat.id(i, 1)+1}(size(C{1, stat.id(i, 1)+1}, 1)+1,1) = stat.TimeUS(i, 1);
    C{2, stat.id(i, 1)+1}(size(C{2, stat.id(i, 1)+1}, 1)+1,1) = stat.stat(i, 1);
end

state = cell2struct(C, FSM_label, 2);

h = axes();
hold on;
leg=[];
plot_count = 0;
color = rand(size(FSM_label, 1), 3);
except = []; % fill except ids here
axis_x = 1;  % 1-n as X axis 2-TimeUS as X axis
times  = 1;  % FSM value times
for i = 1:size(FSM_label, 1)
    if ~isempty(find(FSM(i,1)==except))
        continue
    end
    plot(h, eval(sprintf('%s%d%s%d','state(',axis_x,').id',FSM(i,1))), eval(sprintf('%s%d','state(2).id',FSM(i,1)))*times,'Color',color(i,:),'Marker','o','MarkerFaceColor',color(i,:),'LineStyle','--');
    plot_count = plot_count + 1;
    if(isequal(plot_count,1))
        leg = strcat('''','id',sprintf('%d',FSM(i,1)),'''');
    else
        leg = strcat(leg,',','''','id',sprintf('%d',FSM(i,1)),'''');
    end
end
clc
eval(strcat('legend','(',leg,')'));
grid on
title('Finite State Machine Analysis');