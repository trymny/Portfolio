%**** Last Update 7/11/2019****
%function takes the lidarSLAM object and checks whether there are obstacles
%in the path or not. Uses the coordinates to check if the objects is within
%a certain threshold. If yes, it adds a constant in the opposite direction.

function const = offset(scan, th, parking_mode)

const = 0;
min = closest_point(scan);

if ~parking_mode
    if((min.x < 0) && (min.x > -th.x))
        if((min.y > 0) && (min.y < th.y))
            const = -(th.y-min.y);
        elseif((min.y < 0) && (min.y > -th.y))
            const = +(th.y-min.y);
        end
    end
elseif parking_mode
    if((min.x < 0) && (min.x > -th.x))
        if((min.y > 0) && (min.y < th.y))
            const = -(th.y-min.y)/2;
        elseif((min.y < 0) && (min.y > -th.y))
            const = +(th.y-min.y)/2;
        end
    end
end

end
