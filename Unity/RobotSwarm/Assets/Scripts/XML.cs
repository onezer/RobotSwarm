using System.Collections.Generic;
using System.Xml;
using System.Xml.Serialization;

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
