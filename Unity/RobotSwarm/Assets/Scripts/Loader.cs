using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Xml;
using System.Xml.Serialization;
using System.IO;
using System;

[XmlRoot(ElementName = "size")]
public class Size
{
    [XmlElement(ElementName = "x")]
    public int X { get; set; }
    [XmlElement(ElementName = "y")]
    public int Y { get; set; }
    [XmlElement(ElementName = "z")]
    public int Z { get; set; }
}

[XmlRoot(ElementName = "node")]
public class Node
{
    [XmlElement(ElementName = "x")]
    public int X { get; set; }
    [XmlElement(ElementName = "y")]
    public int Y { get; set; }
    [XmlElement(ElementName = "type")]
    public string Type { get; set; }
}

[XmlRoot(ElementName = "map")]
public class Map
{
    [XmlElement(ElementName = "size")]
    public Size Size { get; set; }
    [XmlElement(ElementName = "node")]
    public List<Node> Node { get; set; }
}

[XmlRoot(ElementName = "iteration")]
public class Iteration
{
    [XmlAttribute(AttributeName = "num")]
    public int Num { get; set; }
    [XmlElement(ElementName = "robot")]
    public List<Robot> Robot { get; set; }
}

[XmlRoot(ElementName = "robot")]
public class Robot
{
    [XmlElement(ElementName = "id")]
    public int Id { get; set; }
    [XmlElement(ElementName = "x")]
    public int X { get; set; }
    [XmlElement(ElementName = "y")]
    public int Y { get; set; }
    [XmlElement(ElementName = "z")]
    public int Z { get; set; }
    [XmlElement(ElementName = "type")]
    public string Type { get; set; }
}

[XmlRoot(ElementName = "simulation")]
public class Simulation
{
    [XmlElement(ElementName = "map")]
    public Map Map { get; set; }
    [XmlElement(ElementName = "iteration")]
    public List<Iteration> Iteration { get; set; }
    [XmlAttribute(AttributeName = "name")]
    public string Name { get; set; }
}

public class Frame
{
    public int num;
    public Dictionary<int,FrameRobot> robots;

    public class FrameRobot
    {
        public int id;
        public Vector3 position;

        public FrameRobot(int id, Vector3 position)
        {
            this.id = id;
            this.position = position;
        }
    }

    

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



public class Loader : MonoBehaviour
{
    public TextAsset Filename;
    public Simulation Simulation;
    public GameObject Tile;
    public GameObject Obstacle;
    public GameObject RobotObj;

    Dictionary<int, Frame> Frames;

    int currentFrame;
    int maxFrame;

    // Start is called before the first frame update
    void Start()
    {
        Simulation = LoadSimulation(Filename.text);
        CreateMap();
        CreateFrames();
        currentFrame = 0;
        maxFrame = Frames.Count-1;
    }

    Simulation LoadSimulation(string path)
    {
        Debug.Log(path);
        try
        {
            XmlSerializer serializer = new XmlSerializer(typeof(Simulation));
            using (FileStream stream = new FileStream(path, FileMode.Open))
            {
                return serializer.Deserialize(stream) as Simulation;
            }
        }
        catch(Exception e)
        {
            Debug.LogError("Exception loading simulation file: " + e.Message);

            return null;
        }
    }

    void CreateMap()
    {
        Map map = Simulation.Map;
        Size size = map.Size;

        for(int x = 0; x < size.X; x++)
        {
            for(int y = 0; y < size.Y; y++)
            {
                Instantiate(Tile, new Vector3(x, 0, y), Quaternion.identity);
            }
        }

        foreach(Node node in map.Node)
        {
            if (node.Type.Contains("obstacle"))
            {
                Instantiate(Obstacle, new Vector3(node.X, (float)0.5, node.Y), Quaternion.identity);
            }
        }
    }

    void CreateFrames()
    {
        Frames = new Dictionary<int, Frame>();

        foreach(Iteration iteration in Simulation.Iteration)
        {
            Frame frame;
            if (iteration.Num > 0)
            {
                frame = new Frame(Frames[iteration.Num - 1]);
            }
            else
            {
                frame = new Frame(iteration.Num);
            }
            
            foreach(Robot robot in iteration.Robot)
            {
                if (robot.Type.Contains("creation"))
                {
                    frame.CreateRobot(robot.Id, new Vector3(robot.X, 0.5f, robot.Y));
                }
                else if (robot.Type.Contains("move"))
                {
                    frame.MoveRobot(robot.Id, new Vector3(robot.X, 0.5f, robot.Y));
                    Debug.Log("move robot " + robot.Id);
                }
            }
            Frames.Add(iteration.Num, frame);
        }
    }

    void DisplayFrame(int num)
    {
        GameObject[] allrobot = GameObject.FindGameObjectsWithTag("Robot");
        foreach(GameObject robot in allrobot)
        {
            GameObject.Destroy(robot);
        }

        Frame currentFrame = Frames[num];
        foreach(KeyValuePair<int, Frame.FrameRobot> entry in currentFrame.robots)
        {
            Instantiate(RobotObj, entry.Value.position, Quaternion.identity);
        }
    }

    // Update is called once per frame
    void Update()
    {
        if (Input.GetKeyDown("right"))
        {
            if(currentFrame < maxFrame)
            {
                currentFrame++;
            }
            DisplayFrame(currentFrame);
        }
        else if (Input.GetKeyDown("left"))
        {
            if(currentFrame > 0)
            {
                currentFrame--;
            }
            DisplayFrame(currentFrame);
        }
    }
}
