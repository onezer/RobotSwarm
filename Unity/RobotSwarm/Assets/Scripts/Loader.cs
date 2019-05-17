using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Xml;
using System.Xml.Serialization;
using System.IO;
using System;
using UnityEditorInternal;


public class Loader : MonoBehaviour
{
    public string Filename;
    public Simulation Simulation;
    public GameObject Tile;
    public GameObject Obstacle;
    public GameObject RobotObj;
    public float Delay;

    float timer;
    bool playing;

    GameObject ButtonImage;

    Dictionary<int, Frame> Frames;

    int currentFrame;
    int maxFrame;

    // Start is called before the first frame update
    void Start()
    {
        Simulation = LoadSimulation(Filename);
        CreateMap();
        CreateFrames();
        currentFrame = 0;
        maxFrame = Frames.Count-1;
        timer = 0;
        DisplayFrame(currentFrame);

        ButtonImage = GameObject.Find("Text");
    }

    // Update is called once per frame
    void Update()
    {
        timer += Time.deltaTime;

        ManageHotkeys();
        Autoplay();
    }

    void Autoplay()
    {
        if (playing == false) return;
        if (timer >= Delay)
        {
            timer = 0;
            if(currentFrame < maxFrame)
            {
                currentFrame++;
                AnimateFrame();
            }
            else
            {
                playing = false;
            }
        }
    }

    void AnimateFrame()
    {
        GameObject[] robots = GameObject.FindGameObjectsWithTag("Robot");

        Dictionary<int, GameObject> robotDictionary = new Dictionary<int, GameObject>();

        Frame frame = Frames[currentFrame];
        
        foreach(GameObject robot in robots)
        {
            int id = robot.GetComponent<RobotId>().id;
            robotDictionary.Add(id, robot);
        }

        foreach(KeyValuePair<int, FrameRobot> entry in frame.robots)
        {
            if (robotDictionary.ContainsKey(entry.Key))
            {
                MoveRobot(robotDictionary[entry.Key], entry.Value.position);
            }
            else
            {
                Instantiate(RobotObj, entry.Value.position, Quaternion.identity).GetComponent<RobotId>().id = entry.Key;
            }
        }
    }

    void MoveRobot(GameObject robot, Vector3 target)
    {
        StartCoroutine(MoveOverSeconds(robot, target, Delay));
    }

    public IEnumerator MoveOverSeconds(GameObject objectToMove, Vector3 end, float seconds)
    {
        if(objectToMove != null)
        {
            float elapsedTime = 0;
            Vector3 startingPos = objectToMove.transform.position;
            while (elapsedTime < seconds)
            {
                if (objectToMove != null)
                {
                    objectToMove.transform.position = Vector3.Lerp(startingPos, end, (elapsedTime / seconds));
                    elapsedTime += Time.deltaTime;
                }
                yield return new WaitForEndOfFrame();
            }
            objectToMove.transform.position = end;
        }
    }

    public void TogglePlay()
    {
        playing = !playing;
    }

    void ManageHotkeys()
    {
        if (Input.GetKeyDown("right"))
        {
            playing = false;
            if (currentFrame < maxFrame)
            {
                currentFrame++;
            }
            DisplayFrame(currentFrame);
        }
        else if (Input.GetKeyDown("left"))
        {
            playing = false;
            if (currentFrame > 0)
            {
                currentFrame--;
            }
            DisplayFrame(currentFrame);
        }
        else if (Input.GetKeyDown("space"))
        {
            TogglePlay();
        }
    }

    Simulation LoadSimulation(string path)
    {
        try
        {
            XmlSerializer serializer = new XmlSerializer(typeof(Simulation));
            using (FileStream stream = new FileStream(path, FileMode.Open))
            {
                return serializer.Deserialize(stream) as Simulation;
            }
        }
        catch (Exception e)
        {
            Debug.LogError("Exception loading simulation file: " + e.Message);

            return null;
        }
    }

    void CreateMap()
    {
        Map map = Simulation.Map;
        Size size = map.Size;

        for (int x = 0; x < size.X; x++)
        {
            for (int y = 0; y < size.Y; y++)
            {
                Instantiate(Tile, new Vector3(x, 0, y), Quaternion.identity);
            }
        }

        foreach (Node node in map.Node)
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

        foreach (Iteration iteration in Simulation.Iteration)
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

            foreach (Robot robot in iteration.Robot)
            {
                if (robot.Type.Contains("creation"))
                {
                    frame.CreateRobot(robot.Id, new Vector3(robot.X, 0.5f, robot.Y));
                }
                else if (robot.Type.Contains("move"))
                {
                    frame.MoveRobot(robot.Id, new Vector3(robot.X, 0.5f, robot.Y));
                }
            }
            Frames.Add(iteration.Num, frame);
        }
    }

    void DisplayFrame(int num)
    {
        GameObject[] allrobot = GameObject.FindGameObjectsWithTag("Robot");
        foreach (GameObject robot in allrobot)
        {
            GameObject.Destroy(robot);
        }

        Frame currentFrame = Frames[num];
        foreach (KeyValuePair<int, FrameRobot> entry in currentFrame.robots)
        {
            Instantiate(RobotObj, entry.Value.position, Quaternion.identity).GetComponent<RobotId>().id = entry.Key;
        }
    }
}
