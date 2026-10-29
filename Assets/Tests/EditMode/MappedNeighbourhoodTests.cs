using System;
using NUnit.Framework;

namespace CodenameCheweenie.Tests
{
    public sealed class MappedNeighbourhoodTests
    {
        private const string Sample = "{\"version\":1,\"roads\":[{\"name\":\"Sample Street\",\"width\":8,\"points\":[{\"x\":0,\"z\":-30},{\"x\":100,\"z\":-30}]}],\"stops\":[{\"stop\":1,\"x\":0,\"z\":-30},{\"stop\":2,\"x\":30,\"z\":-30},{\"stop\":4,\"x\":50,\"z\":-30},{\"stop\":16,\"x\":70,\"z\":-30},{\"stop\":8,\"x\":0,\"z\":-23}],\"tour\":[{\"x\":0,\"z\":-23},{\"x\":70,\"z\":-30,\"stop\":16}],\"school\":{\"x\":70,\"z\":-30,\"label\":\"Sample School\"}}";
        [Test] public void ReadsPrivateMapWithoutRequiringGeographicCoordinatesOrAnAddress()
        {
            var data = MappedNeighbourhood.Parse(Sample);
            Assert.IsTrue(data.IncludesSchool); Assert.AreEqual(70, data.Find(NeighbourhoodStop.School).x);
            Assert.AreEqual(16, data.tour[1].stop); Assert.Greater(data.Bounds.width, 100);
        }
        [Test] public void RejectsUnsupportedSchema()
        { Assert.Throws<ArgumentException>(() => MappedNeighbourhood.Parse(Sample.Replace("\"version\":1", "\"version\":2"))); }
        [Test] public void RejectsOutOfBoundsCoordinates()
        { Assert.Throws<ArgumentException>(() => MappedNeighbourhood.Parse(Sample.Replace("\"x\":100", "\"x\":10000"))); }
        [Test] public void RejectsDuplicateOrMissingPatrolStops()
        { Assert.Throws<ArgumentException>(() => MappedNeighbourhood.Parse(Sample.Replace("\"stop\":2", "\"stop\":1"))); }
        [Test] public void RejectsInvalidRoadWidths()
        { Assert.Throws<ArgumentException>(() => MappedNeighbourhood.Parse(Sample.Replace("\"width\":8", "\"width\":0"))); }
        [Test] public void RejectsASchoolStopThatDoesNotReachTheMappedSchoolSite()
        {
            string json = Sample.Replace("\"school\":{\"x\":70,\"z\":-30", "\"school\":{\"x\":1000,\"z\":1000");
            Assert.Throws<ArgumentException>(() => MappedNeighbourhood.Parse(json));
        }
    }
}
