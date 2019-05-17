using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Frame
{
    public int num;
    public Dictionary<int, FrameRobot> robots;

    public Frame(int num)
    {
        robots = new Dictionary<int, FrameRobot>();
        this.num = num;
    }

    public Frame(Frame other)
    {
        num = other.num + 1;
        robots = new Dictionary<int, FrameRobot>(other.robots);
    }

    public void CreateRobot(int id, Vector3 position)
    {
        robots.Add(id, new FrameRobot(id, position));
    }

    public void MoveRobot(int id, Vector3 position)
    {
        robots[id] = new FrameRobot(id, position);
    }
}


