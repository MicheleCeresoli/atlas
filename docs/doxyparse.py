import os 
import xml.etree.ElementTree as et 

# Path to Doxygen's XML output 
doxygen_xml_path = os.path.join("..", "build", "docs", "xml")

# Path to the directory in which to put the XML 
rst_output_path = os.path.join("source", "api", "cpp")

# Ensure the output direcotry exists 
os.makedirs(rst_output_path, exist_ok=True)

# File in which to add the Class list
toc_file = os.path.join('source', 'api', 'api_cpp.rst')

# Start writing the ToC
toc_header = [
    "C++ API", "====================", "", "", 
]

# toc_hidden = [".. toctree::", "   :hidden:", ""]
toc_content = ["", "", "Class List:", "---------------------"]

# Loop through all the XML files in Doxygen output 
class_files = []

for filename in os.listdir(doxygen_xml_path): 
    if filename.startswith("class") and filename.endswith(".xml"): 
        class_files.append(filename)

# Sorth the classes alphabetically        
class_files.sort()

for filename in class_files:
    
    xml_path = os.path.join(doxygen_xml_path, filename)
    
    # Parse the XML file 
    tree = et.parse(xml_path)
    root = tree.getroot()
    
    # Find the class name 
    compoundname = root.find(".//compoundname")
    
    if compoundname is not None: 
        # Retrieve class name
        class_name = compoundname.text.strip() 
        
        # Create a safe filename for the class 
        rst_filename = f"{class_name}.rst"
        rst_filepath = os.path.join(rst_output_path, rst_filename)
        
        # Generate the .rst content for the class 
        rst_content = [
            class_name, 
            "=" * len(class_name), 
            "", 
            f".. doxygenclass:: {class_name}", 
            "   :members:", 
            ""
        ]
        
        # toc_hidden.append(f"   cpp/{class_name}.rst")
        
        # Write the .rst file 
        with open(rst_filepath, "w") as rst_file: 
            rst_file.write("\n".join(rst_content))
            
        # Add entry to the Toc 
        toc_content.append(f"* :doc:`{class_name} <cpp/{class_name}>`")
        
        
# Write the updated index.rst (Toc)
with open(toc_file, "w") as toc: 
    toc.write("\n".join(toc_header))
    # toc.write("\n".join(toc_hidden))
    toc.write("\n".join(toc_content))
    
print("Converted Doxygen C++ .xml to .rst")