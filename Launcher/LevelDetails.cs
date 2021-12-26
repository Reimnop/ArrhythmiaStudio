using Newtonsoft.Json.Linq;
using System;

namespace Launcher
{
    public struct LevelDetails
    {
        public string LevelName;
        public string LevelPath;
        public DateTime LastOpened;

        public LevelDetails(JToken json)
        {
            LevelName = (string)json["name"];
            LevelPath = (string)json["path"];
            LastOpened = (DateTime)json["last_opened"];
        }

        public JObject ToJson()
        {
            JObject json = new JObject();
            json["name"] = LevelName;
            json["path"] = LevelPath;
            json["last_opened"] = LastOpened;
            return json;
        }
    }
}
