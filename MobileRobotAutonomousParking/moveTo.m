%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% SKKU EME3071
% ------------
%This function is modified so that it can be called within the parking.m
%function. This function takes the current position iQ, PID parameters for PID and
%the column number of the trajectory to avoid index being higher value than
%number of columns. In the end, returns the values of index and vel_msg.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function [vel_msg,i]= moveTo(iQ,iP,PID,i,LastPoint)
    % Circular mask for termination condition
    % Unit: [m]
    Look_Ahead = 0.02;  % 2cm

    %=========== Parameters ===========
    max_linear_vel = 0.16;       % [m/sec]
    wheel_interval = 0.158;      % 2L [m]
    L = wheel_interval / 2.0;
    max_rotational_vel = 2.5;   % [rad/sec]

    %=========== Mobile Robot Kinematics ===========
    % Transform from "I" coordinate to "R" coodinate
    iD = [ (iP(1)-iQ(1)); (iP(2)-iQ(2)) ];  % iP - iQ

    ri_R = [ cos(iQ(3)), sin(iQ(3));
            -sin(iQ(3)), cos(iQ(3))];       % Rotation matrix "R" to "I"

    rP = ri_R * iD;                         % We need rP!

    theta = atan2(rP(2), rP(1));            % arctan2(Y,X)

    %=========== PID Controller ===========
    % P-control Example
    linear_velocity  = PID(1,1) * rP(1);        % Kx_p * rP_x
    angular_velocity = PID(2,1) * theta;        % Kh_p * rP_theta

    % Constraints
    linear_velocity = min(linear_velocity, max_linear_vel);
    linear_velocity = max(linear_velocity, -max_linear_vel);
    angular_velocity = min(angular_velocity, max_rotational_vel);
    angular_velocity = max(angular_velocity, -max_rotational_vel);

    %***updated on 7/11/2019, we could need to limit the values of the
    %velocities to protect the robot

    %=========== Velocity -> Commend ===========
    left_wheel  = linear_velocity - (L * angular_velocity);
    right_wheel = linear_velocity + (L * angular_velocity);

    %=========== Publishing ===========
    distance = sqrt( rP(1)*rP(1) + rP(2)*rP(2) ); % Using 'square root'

    if distance < Look_Ahead  % Termination condition
        vel_msg.X = 1;
        vel_msg.Y = 0;
        vel_msg.Z = 0;
        return
    else
        vel_msg.X = 1;
        vel_msg.Y = left_wheel;
        vel_msg.Z = right_wheel;
    end

end