#!/usr/bin/env python

"""
GUI display for ballbot
Publishes to topics:  goal,obstacles
Subscribes to topics: path,odom
"""
import roslib; roslib.load_manifest('LatticePlanner')
import rospy
import graphics
import math
import util
from Tkinter import *

from std_msgs.msg import String
from bb_msgs.msg import Pose,Path

pub_goal = rospy.Publisher('goal',Pose)
Ballbot_x = 0.0
Ballbot_y = 0.0
Ballbot_theta = 0.0
Ballbot_Tkobjects = None

Goal_x = 0.0
Goal_y = 0.0

def startPlanner(x1,y1,th1,d_goal,th_goal):
    """
    Send a goal message to topic 'goal'
    """

    """
    Draw court
    """
    global Goal_x,Goal_y

    graphics.canvas.delete(ALL)
    #util.costmap.draw_costmap()
    graphics.draw_court()    
    th1 = math.radians(th1)    
    
    th_goal = th1 - math.radians(th_goal)     # angle to goal in global coord
                                              # = angle of car in global coord - angle to goal from car (which is in [-90deg,90deg]
    x2 = x1 + d_goal*math.cos(th_goal)
    y2 = y1 + d_goal*math.sin(th_goal)
    th2 = th1                                   # doesn't matter for goal test
    
    pub_goal.publish(x2,y2,th2)
    Goal_x = x2
    Goal_y = y2

def received_odometry(data):
    global Ballbot_x,Ballbot_y,Ballbot_theta,Ballbot_Tkobjects
    Ballbot_x = data.x
    Ballbot_y = data.y
    Ballbot_theta = data.theta
    """
    delete old car
    """
    if(Ballbot_Tkobjects!=None):
        graphics.canvas.delete(Ballbot_Tkobjects[0])
        graphics.canvas.delete(Ballbot_Tkobjects[1])
    """
    redraw car at new position
    """
    Ballbot_Tkobjects = graphics.draw_car(data.x*100.0,data.y*100.0,data.theta)

def received_path(data):
    """
    draw the new path
    """
    graphics.canvas.delete(ALL)
    graphics.draw_court()    
    graphics.draw_point(Goal_x,Goal_y,0,color='red')
    
    
    for pose in data.poses:        
        graphics.draw_point(pose.x*100.0,pose.y*100.0,pose.theta,color='green')    
        #print "drew",pose.x*100.0,pose.y*100.0,pose.theta
    graphics.draw_point_directed(data.poses[0].x*100.0,data.poses[0].y*100.0,data.poses[0].theta)
    graphics.draw_point_directed(data.poses[-1].x*100.0,data.poses[-1].y*100.0,data.poses[-1].theta)
        
    graphics.canvas.update_idletasks()
    print "drew path"

def initialize_gui():    
    rospy.init_node('GUI')
    while not rospy.is_shutdown():
        root = graphics.root
        root.title("Lattice Planner")
        root.configure(background = 'grey')
        graphics.canvas.pack()

        # x initial
        entryLabel_x1 = Label(root)
        entryLabel_x1["text"] = "init: X"
        entryLabel_x1.pack(side = LEFT)
        entryWidget_x1 = Entry(root)
        entryWidget_x1["width"] = 5
        entryWidget_x1.pack(side = LEFT)

        # y initial
        entryLabel_y1 = Label(root)
        entryLabel_y1["text"] = "Y"
        entryLabel_y1.pack(side = LEFT)
        entryWidget_y1 = Entry(root)
        entryWidget_y1["width"] = 5
        entryWidget_y1.pack(side = LEFT)

        # theta initial
        entryLabel_th1 = Label(root)
        entryLabel_th1["text"] = "TH"
        entryLabel_th1.pack(side = LEFT)
        entryWidget_th1 = Entry(root)
        entryWidget_th1["width"] = 5
        entryWidget_th1.pack(side = LEFT)

        # d goal (cm)
        entryLabel_d = Label(root)
        entryLabel_d["text"] = "    goal: d"
        entryLabel_d.pack(side = LEFT)
        entryWidget_d = Entry(root)
        entryWidget_d["width"] = 5
        entryWidget_d.pack(side = LEFT)

        # theta goal
        entryLabel_th = Label(root)
        entryLabel_th["text"] = "TH"
        entryLabel_th.pack(side = LEFT)
        entryWidget_th = Entry(root)
        entryWidget_th["width"] = 5
        entryWidget_th.pack(side = LEFT)
    
        b = Button(root,text = "Go",command = lambda: startPlanner(float(entryWidget_x1.get()),float(entryWidget_y1.get()),float(entryWidget_th1.get()),float(entryWidget_d.get()),float(entryWidget_th.get())))
        g = Button(root,text = "Show lattice",command = lambda:graphics.draw_lattice())
        g.pack(side = RIGHT)
        b.pack(side = RIGHT)
        #util.costmap.draw_costmap()
        graphics.draw_court()

        graphics.canvas.configure(cursor = "crosshair")
        #graphics.canvas.bind("<Button-1>",obstacle_added)

        rospy.Subscriber("odometry",Pose, received_odometry)
        rospy.Subscriber("path",Path,received_path)
        root.mainloop()

if __name__ == '__main__':
    try:
       initialize_gui() 
    except rospy.ROSInterruptException: pass
