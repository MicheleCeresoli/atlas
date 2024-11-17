import os 
import subprocess

def setup(app): 
    app.connect("builder-inited", generate_doxygen_api)
    
def generate_doxygen_api(app): 
    subprocess.run(["python3", "doxyparse.py"])


# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

project = 'atlas'
copyright = '2024, Michele Ceresoli'
author = 'Michele Ceresoli'
release = '0.4.3'

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = [
    'breathe',
    'sphinx-prompt'
]

breathe_projects = {
    "atlas": "../../build/docs/xml"
}

breathe_default_project = "atlas"

templates_path = ['_templates']
exclude_patterns = []


# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = 'sphinx_rtd_theme'
html_static_path = ['_static']

html_title = "atlas"
