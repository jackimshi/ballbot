#!/usr/bin/env python
import math
import util
import time
import controlset
from Tkinter import *

import roslib; roslib.load_manifest('LatticePlanner')
import rospy
from std_msgs.msg import String
from bb_msgs.msg import Pose,Path

startNode = None
goalNode = None
plan = [] # stores the computed plan, as [(node1,action1),(node2,action2)....]
path = [] # stores the computed path, as a sequence of (x,y,theta) values

pub_path = rospy.Publisher('path',Path)
Ballbot_x = 350.0
Ballbot_y = 350.0
Ballbot_theta = math.pi/2

def received_odometry(data):
    global Ballbot_x,Ballbot_y,Ballbot_theta
    Ballbot_x = data.x*100.0
    Ballbot_y = data.y*100.0
    Ballbot_theta = data.theta

def startPlanner(data):
    global startNode,goalNode,plan,path

    (x1,y1,th1,v) = util.point_to_lattice(Ballbot_x,Ballbot_y,Ballbot_theta,util.ROBOT_SPEED_MAX)    
 
    v2 = util.ROBOT_SPEED_MAX
    (x2,y2,th2,v2) = util.point_to_lattice(data.x,data.y,data.theta,util.ROBOT_SPEED_MAX)
    
    if(x1 < 0) or (x1 > util.COURT_WIDTH) or (y1 < 0) or (y1 > util.COURT_LENGTH):
        print "Invalid start!"
        return
    if(x2 < 0) or (x2 > util.COURT_WIDTH) or (y2 < 0) or (y2 > util.COURT_LENGTH):
        print "Invalid goal!"
        return    

    startNode = util.LatticeNode(stateparams = (x1,y1,th1,v))
    goalNode  = util.LatticeNode(stateparams = (x2,y2,th2,v2))    
    
    print "start ",startNode.get_stateparams()," goal ",goalNode.get_stateparams()
    (x1,y1,th1,v1) = startNode.get_stateparams()
    (x2,y2,th2,v2) = goalNode.get_stateparams()

    if (util.SEARCHALGORITHM == "A*"):
        Astarsearch(startNode,goalNode)
    elif (util.SEARCHALGORITHM == "LPA*"):
        print "running LPA*"
        LPAstarsearch(startNode,goalNode)
    elif (util.SEARCHALGORITHM == "MT-AdaptiveA*"):
        print "running MT-AdaptiveA*"
        MTAdaptiveAstarsearch(startNode,goalNode)
        
    print "Hit enter to drive along path"
    raw_input()    
    path.append((x1/100.0,y1/100.0,th1))
    path = path + util.plan_to_path(plan)
    print path
    print "Hit enter to continue"
    raw_input()    

    path_to_send = Path()    
    for point in path:
        pose = Pose()
        pose.x = point[0]
        pose.y = point[1]
        pose.theta = point[2]
        path_to_send.poses.append(pose)

    pub_path.publish(path_to_send)
    print "path published"

def Astarsearch(startNode,goalNode):
    global plan
    (x1,y1,th1,v1) = startNode.get_stateparams()
    time_exec = time.time()
    goalNode = util.Astarsearch(startNode,goalNode)    
    if(goalNode != None):
        print "found goal! in",time.time() - time_exec,"s"            
        node = goalNode
        while(node.getParent()!= None):
            parent = node.getParent()
            #print parent.get_stateparams(),node.getAction(),node.get_stateparams()
            #print node.getAction(),util.controlset.len_action(node.getAction())
            plan.append((parent,node.getAction()))                   
            node = parent  
        print ""            
        plan.reverse()
        (x2,y2,th2,v2) = goalNode.get_stateparams()                        
    else:
        print "No path to goal!"

def LPAstarsearch(startNode,goalNode):
    global plan   

    # draw plan        
    for (node,action) in util.plan_LPAstar:
        graphics.draw_segment(node,action)   

def MTAdaptiveAstarsearch(startNode,goalNode):
    global plan    
    plan = util.MTAdaptiveAstarsearch(startNode,goalNode)    
        

def obstacle_added(event):        
    util.costmap.new_obstacle(event)
    if(startNode != None) and (goalNode != None):
        graphics.canvas.delete(ALL)
        util.costmap.draw_costmap()
        graphics.draw_court() 

        # draw plan
        for (node,action) in util.plan_LPAstar:            
            graphics.draw_segment(node,action)

        (start_x,start_y,start_th,start_v) = startNode.get_stateparams()
        (goal_x,goal_y,goal_th,goal_v) = goalNode.get_stateparams()
        graphics.draw_point_directed(start_x,start_y,start_th,'red')
        graphics.draw_point_directed(goal_x,goal_y,goal_th,'red')


def init_planner():
    rospy.init_node('LatticePlanner', anonymous=True)
    rospy.Subscriber("goal", Pose, startPlanner)
    rospy.Subscriber("odometry",Pose,received_odometry)
    rospy.spin()


if __name__ == '__main__':
    try:
        util.controlset = controlset.ControlSet()
        util.costmap = util.CostMap()
        init_planner()
    except rospy.ROSInterruptException: pass

