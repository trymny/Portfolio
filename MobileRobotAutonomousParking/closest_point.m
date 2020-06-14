%**** Last Update 5/11/2019****
%Function takes the lidarSLAM object and finds the closest point in the
%map. Function runs through all data points that corresponds to each
%degree and finds the closest one. 

function min = closest_point(scan)
    min.dist = max(scan.Ranges);
    for j = 1:1:size(scan.Ranges)
        if(scan.Angles(j) > -pi && scan.Angles(j) < 0)
            if(scan.Ranges(j) < min.dist && scan.Ranges(j) ~= 0)
                min.dist = scan.Ranges(j);
                min.x = scan.Cartesian(j,1);
                min.y = scan.Cartesian(j,2);
            end
        end
    end
end
