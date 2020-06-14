%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% SKKU EME3071
% ------------
% Last Update : 7/11/2019
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

rosshutdown;            % Clean up
rosinit('192.168.4.1'); % Initialize the node

%=========== Parameters ===========
% http://emanual.robotis.com/docs/en/platform/turtlebot3/specifications/#hardware-specifications
wheel_radius = 0.0325;       % [m]
wheel_interval = 0.158;      % 2L [m]
L = wheel_interval / 2.0;

%Threshold Values
th.x = 0.60;           % [m]
th.y = 0.40;           % [m]


%Lidar Parameters
minLidarRange = 0.12;  % minimum range of lidar
maxLidarRange = 3;   % maximum range of lidar
mapResolution = 50;    % Resolution of the map. (unit: cell per meter)

const = 0;
%=========== Subscribers ===========
pose_sub = rossubscriber('/odom');
lidar_sub = rossubscriber('/scan');
pause(1);  % Ready for the first receiving

%=========== Publishers ===========
% The inside of '/wheel_control' topic:
%     X: Operation modes of dynamixel (1:velocity mode, 16:pwm mode)
%     Y: Left motor    [m/s] or (-1.0 ~ 1.0)
%     Z: Right motor   [m/s] or (-1.0 ~ 1.0)
vel_pub = rospublisher('/wheel_control', 'geometry_msgs/Vector3');
vel_msg = rosmessage(vel_pub);

% Operation modes
VELOCITY_MODE = 1;
PWM_MODE = 16;

%=========== For PID ===========
% X error
Kx_p = 1.0;
Kx_i = 0;
Kx_d = 0;

% Heading error (theta)
Kt_p = 3.0;
Kt_i = 0;
Kt_d = 0;

PID=[Kx_p,Kx_i,Kx_d; Kt_p,Kt_i,Kt_d];

%=========== Following a Trajectory ===========
% Target point "P" in the coordinate system "I"
%Unit: [m]

% x_coord = [0.00 1.00 1.00 0.00 0.00];
% y_coord = [0.00 0.00 1.00 1.00 0.00];
% 
% iP = [x_coord; -y_coord];

%=========== Parking ===========
% Target point "P" in the coordinate system "I"
% Unit: [m]

x_coord = [0.00 0.00 0.00 0.00 0.00 0.00 0.00];
y_coord = [0.00 -0.1 -0.2 -0.3 -0.4 -0.5 -0.6];

iP = [x_coord; 2*y_coord];

%======== Origin of the world frame ======
%To find the origin of the world frame comment out this part and comment
%the upper part
% Target point "P" in the coordinate system "I"
% Unit: [m]

% x_0 = [0.0 0.0 0.0];
% y_0 = [0.0 0.-0.5 -0.6];
% 
% iP = [x_0; y_0];

%======== Indices ========
i = 1;
k = 1;

[row, column] = size(iP);
LastPoint = column;
%=========== Loop ===========
rate = rosrate(20);  % 20Hz
reset(rate);
initialTime = rostime("now");
parking_mode = false; 


scan = SLAM(lidar_sub, minLidarRange, maxLidarRange, mapResolution);
waitfor(0.1);

while true
    % Read the data by subscriber
    %=========== topic: '/odom' ===========
    position = pose_sub.LatestMessage.Pose.Pose.Position;
    quat = pose_sub.LatestMessage.Pose.Pose.Orientation;
    euler_angle = quat2eul([quat.X quat.Y quat.Z quat.W]);
    Heading = euler_angle(3);  % Current Heading [radian]
    
    % Current pose of the robot "Q" in the coordinate system "I"
    %     [Comment]: Using "R" as the robot symbol can be confusing
    %                with the rotation matrix. So we use Q here.
    iQ = [position.X, position.Y, Heading];
    
    
    %======== offset to avoid obstacles
    
    %const = offset(scan, th);

    %=========Following a Trajectory by using the moveTo function
    [msg,i] = moveTo(iQ,iP(:,i),PID,i,LastPoint);
    
    
    % To send the message to the robot
    vel_msg.X = msg.X;
    vel_msg.Y = msg.Y;
    vel_msg.Z = msg.Z;
    
    send(vel_pub,vel_msg);
    
    %========== Collecting the data ==========   
    % Following lines are added to collect the position and velocity values
    % of the current position.
    
    collect(1,k) = position.X;
    collect(2,k) = position.Y;
    slam_data(1,i) = scan;

    k = k+1;
    %=========calling SLAM function and updating index
    
    if i < LastPoint && vel_msg.Y ==0 && vel_msg.Z==0  % Updates the index if it is not the last point
        i=i+1;
        scan = SLAM(lidar_sub, minLidarRange, maxLidarRange, mapResolution);
        const = offset(scan, th);
        iP(2,i) = iP(2,i) + const;
        if i < column-1
            iP(2,i+1) = iP(2,i+1) + const;
        end
    end
    
    
    if i < LastPoint && i > LastPoint - 5
        parking_mode = true;
    end
    
    %==================Debuging==================
    disp("*****offset_value*****"); disp(const);
    disp("position"); disp(position);    

    waitfor(rate);    
end