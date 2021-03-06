#!/usr/bin/env python
"""
controller.py - implements a controller to make Ballbot drive along the planned path
                When the message "ValidPath" is seen on the 'ValidPath' topic, a valid path has been computed/recomputed and must be followed 
                The most recent path is stored in util.path
"""
import roslib; roslib.load_manifest('lattice_planner')
import rospy
import util
import math
from std_msgs.msg import String
from bb_msgs.msg import Pose,DriveCmd,Path

path = None
newPath = False

# ODOMETRY inputs
Ballbot_X = 0
Ballbot_Y = 0
Ballbot_TH = 0

# CONTROL outputs
Ballbot_steering = 0 # radians
Ballbot_speed = 0    # m/s

# CONTROLLER PARAMETRS
targetlookahead = 25.0 # PD steering control to reach a moving target 15 cm ahead of the car

pub_velcmd = rospy.Publisher('vel_cmd', DriveCmd)
pub_status = rospy.Publisher('status', String)

def nearestNeighbor_inPath((x,y,th),currentindex_inPath):
    """
    Given x,y,th, find the point closest to the robot in the path
    """
    global path
    d_min = float('inf')
    index_min = 0
    index = currentindex_inPath
    for path_element in path[currentindex_inPath:currentindex_inPath+10]:
        """
        Search only from the currentindex to currentindex+10
        """
        d = util.distance_Euclidean(path_element.pose.x,path_element.pose.y,x,y)
        if(d < d_min):
            d_min = d
            index_min = index
        elif(d < 0.05): # if error is less than 5 cm, take this as the nearest point
            d_min = d
            index_min = index
            break
        index +=1
    return index_min


def controller_PD():
    """
    Implements controller
    """    
    global path,newPath,Ballbot_steering,Ballbot_speed,pub_velcmd
    Ballbot_speed = 0.0
    Ballbot_steering = 0.0
    currentindex_inPath = 0 # path index that the car is closest to
    targetindex_inPath = 0  # path index that is 50 cm ahead of the car
    # PID stuff
    # -- tuning parameters
    steering_P = 1.0
    steering_D = 0.1    
    # -- state parameters
    error = 0.0
    theta_old = 0.0 # stores previous theta value for D-term
    r = rospy.Rate(60)
    while not rospy.is_shutdown():
        if newPath == False:
            continue
        while(currentindex_inPath < len(path)-1) and not (rospy.is_shutdown()):
            Ballbot_speed = 1.0 # set speed                   
            if(newPath == True):
                # if there is a new path, restart driving along this path
                newPath = False
                currentindex_inPath = 0
                targetindex_inPath = 0
                theta_old = Ballbot_TH
                continue
            else:                                    
                currentindex_inPath = nearestNeighbor_inPath((Ballbot_X,Ballbot_Y,Ballbot_TH),currentindex_inPath)
                targetindex_inPath = min(len(path)-1,currentindex_inPath + int(targetlookahead/5.0)) # points are at a separation of 5 cm                
                #print "currentindex",currentindex_inPath,"targetindex",targetindex_inPath
                # P - Proportional term               
	        #print "error",error

                # P - Proportional term                

                heading = (math.atan2(path[targetindex_inPath].pose.y-Ballbot_Y, path[targetindex_inPath].pose.x-Ballbot_X)%(2*math.pi))
                error = Ballbot_TH - heading    
                """
                correct roll-over problems with error:
                if abs(error) is greater than 180, then we'd rather turn the other way!
                """
                if abs(error) > math.pi:
                    error = (2*math.pi - abs(error))*(-1*cmp(error,0))                          		
                Pterm = steering_P * error                
                # D - Differential term
                Dterm = steering_D * (Ballbot_TH - theta_old)
                theta_old = Ballbot_TH
                # set output
                Ballbot_steering = Pterm - Dterm
                if(Ballbot_steering > math.radians(30)):
                    Ballbot_steering = math.radians(30)
                elif(Ballbot_steering < -math.radians(30)):
                    Ballbot_steering = -math.radians(30)
		#print "error",error,"steering",Ballbot_steering
                pub_velcmd.publish(Ballbot_speed,Ballbot_steering)
            r.sleep()	        
        # goal reached!
        print "goalreached"
        Ballbot_speed = 0
        Ballbot_steering = 0
        currentindex_inPath = 0
        targetindex_inPath = 0
        pub_velcmd.publish(Ballbot_speed,Ballbot_steering)                    
	
def controller_Stanley():
    """
    Steering control is similar to that used by Stanford's Stanley (DARPA Grand Challenge winner)
    """
    global path,newPath,Ballbot_steering,Ballbot_speed,pub_velcmd
    k = 2.0
    Ballbot_speed = 0.0
    Ballbot_steering = 0.0
    currentindex_inPath = 0 # path index that the car is closest to
    targetindex_inPath = 0  # path index that is 50 cm ahead of the car
    r = rospy.Rate(10)    
    average_error_position = 0
    ctr_error_position = 0
    while not rospy.is_shutdown():
        if newPath == False:
            continue
        while(currentindex_inPath < len(path)-1) and not (rospy.is_shutdown()):
            Ballbot_speed = 1.0 # set speed                   
            if(newPath == True):
                # if there is a new path, restart driving along this path
                newPath = False
                currentindex_inPath = 0                               
                targetindex_inPath = 0
                average_error_position = 0
                ctr_error_position = 0
                continue
            else:
                currentindex_inPath = nearestNeighbor_inPath((Ballbot_X,Ballbot_Y,Ballbot_TH),currentindex_inPath)
                targetindex_inPath = min(len(path)-1,currentindex_inPath + int(targetlookahead/5.0))
                path_element = path[currentindex_inPath]

                # calculate cross-track error, x_t
                x_t = util.distance_Euclidean(Ballbot_X,Ballbot_Y,path_element.pose.x,path_element.pose.y)

                average_error_position += x_t
                ctr_error_position +=1
                heading = (math.atan2(path[targetindex_inPath].pose.y-Ballbot_Y, path[targetindex_inPath].pose.x-Ballbot_X)%(2*math.pi))
                error = Ballbot_TH - heading
                """
                correct roll-over problems with error:
                if abs(error) is greater than 180, then we'd rather turn the other way!
                """
                if abs(error) > math.pi:
                    error = (2*math.pi - abs(error))*(-1*cmp(error,0))
                if error < 0:
                    x_t = -1*x_t                
                # calculate heading error, psi_t
                psi_t = Ballbot_TH - path_element.pose.theta               
                """
                correct roll-over problems with error:
                if abs(error) is greater than 180, then we'd rather turn the other way!
                """
                if abs(psi_t) > math.pi:
                    psi_t = (2*math.pi - abs(psi_t))*(-1*cmp(psi_t,0))                    
                # Set steering
                Ballbot_steering = psi_t + math.atan(k*x_t/Ballbot_speed)
                if(Ballbot_steering > math.radians(30)):
                    Ballbot_steering = math.radians(30)
                elif(Ballbot_steering < -math.radians(30)):
                    Ballbot_steering = -math.radians(30)
                # Speed control
                cur_dir = path_element.direction
                lookahead_dir = path[targetindex_inPath].direction
                cur_type = path_element.type
                lookahead_type = path[targetindex_inPath].type
                near_obstacle = path_element.near_obstacle            
                Ballbot_speed = 1.0 # 2.0 
                if(cur_type=='t') or (lookahead_type=='t'):
                    Ballbot_speed = 1.0                
                if(near_obstacle == "t"):                    # if near obstacle, slow down
                    Ballbot_speed = 1.0
                if(cur_dir != lookahead_dir):                # if a reverse is coming up, slow down
                    Ballbot_speed = 0.5                
                if(currentindex_inPath >= (len(path) - 20)): # if within 1 m of goal, slow down
                    Ballbot_speed = 1.0
                if(cur_dir == 'b'):                          # flip sign to reverse
                    Ballbot_speed = -1*abs(Ballbot_speed)
                elif(cur_dir == 'f'):
                    Ballbot_speed = abs(Ballbot_speed)
                else:
                    Ballbot_speed = 0.0
                
                #Ballbot_speed = 0.0;  # UNCOMMENT TO SET DRIVE SPEED to 0
                pub_velcmd.publish(Ballbot_speed,Ballbot_steering)
            r.sleep()
        # goal reached!
        rospy.loginfo("goalreached")
        pub_status.publish("goalreached")        
        Ballbot_speed = 0
        Ballbot_steering = 0
        currentindex_inPath = 0
        targetindex_inPath = 0
        pub_velcmd.publish(Ballbot_speed,Ballbot_steering)         
        #final_error_position = util.distance_Euclidean(Ballbot_X,Ballbot_Y,path[-1].pose.x,path[-1].pose.y)
        #final_error_angle = abs(Ballbot_TH - path[-1].pose.theta)
        #average_error_position = average_error_position/ctr_error_position
        #rospy.loginfo("final_error_pos %f final_error_angle %f average_error_pos %f",final_error_position,final_error_angle,average_error_position)

def newPath_arrived(data):
    """
    A new path has been computed. Feed it into path and set newPath to true
    """
    global path,newPath
    path = data.poses

    print "newpathseen! length",len(path)        
    #print "hit a key to drive"
    #raw_input()
    newPath = True

def received_odometry(data):
    """
    Update pose of the car
    """
    global Ballbot_X,Ballbot_Y,Ballbot_TH
    # Coordinate frame conversion from localization frame to planner frame
    Ballbot_X = (data.y + 3.658)*100.0
    Ballbot_Y = (30.17 - data.x)*100.0
    Ballbot_TH = (data.theta - math.pi/2)%(2*math.pi)

def listener():
    rospy.init_node('controller',anonymous = True)
    rospy.Subscriber('path', Path, newPath_arrived)    
    rospy.Subscriber('pose', Pose, received_odometry)
    #controller_PD() 
    controller_Stanley()
    rospy.spin()
    
def shdn():
    """
    When controller is shut down, send steering = 0, drive = 0
    """
    pub_velcmd.publish(0.0,0.0)
    
if __name__ == '__main__':
    try:
	rospy.on_shutdown(shdn)	
        listener()
    except rospy.ROSInterruptException: pass
