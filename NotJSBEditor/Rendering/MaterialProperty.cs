namespace NotJSBEditor.Rendering
{
    public struct MaterialProperty
    {
        public string Name;
        public MaterialPropertyType PropertyType;
        public int Size;

        public MaterialProperty(string name, MaterialPropertyType propertyType, int size)
        {
            Name = name;
            PropertyType = propertyType;
            Size = size;
        }
    }
}
