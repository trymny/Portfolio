%******Last update 7/11/2019

function mapping = SLAM(lidar_sub, minLidarRange, maxLidarRange, mapResolution)

slamObj = robotics.LidarSLAM(mapResolution, maxLidarRange); % SLAM Object
slamObj.LoopClosureThreshold = 200;
slamObj.LoopClosureSearchRadius = 10;

for i=1:1:5
    ranges = lidar_sub.LatestMessage.Ranges;          % Subscribe scanning data
    angles = linspace(-pi, pi, numel(ranges)); 	      % The angles of scanned data
    scan = lidarScan(double(ranges), double(angles)); % Object for storing 2D scan
    
    % Remove invalid data (out of range)
    valid_scan = removeInvalidData(scan, 'RangeLimits', [minLidarRange, maxLidarRange]);
    
    % Add scanned data into SLAM obj.
    [isScanAccepted, loopClosureInfo, optimizationInfo] = addScan(slamObj, valid_scan);
    figure(1); show(slamObj);
    title('SLAM')
    
    [scansSLAM, poses] = scansAndPoses(slamObj);
    % Occupancy map generated from SLAM
    occupancyGrid = buildMap(scansSLAM, poses, mapResolution, maxLidarRange);
    %  figure(2); show(occupancyGrid);
    %  title('Occupancy Map')
    
    pause(0.01);
end

mapping = valid_scan;
end