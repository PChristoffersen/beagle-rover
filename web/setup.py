from setuptools import setup, find_packages
from pathlib import Path


with open("README.md", "r", encoding="utf-8") as fh:
    long_description = fh.read()

def read(rel_path: str) -> str:
    here = Path(__file__).parent.absolute()
    file = here / rel_path
    with file.open("r") as f:
        return f.read()

def get_version(rel_path: str) -> str:
    for line in read(rel_path).splitlines():
        if line.startswith('__version__'):
            delim = '"' if '"' in line else "'"
            return line.split(delim)[1]
    else:
        raise RuntimeError("Unable to find version string.")

def glob_fix(package_name, glob):
    # this assumes setup.py lives in the folder that contains the package
    package_path = Path(f'./src/{package_name}').resolve()
    return [str(path.relative_to(package_path)) 
            for path in package_path.glob(glob)]

setup(
    name="robotweb",
    version=get_version("src/robotweb/__init__.py"),
    author="Peter Christoffersen",
    author_email="pch@plyt.dk",
    description="Web UI for robot",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/PChristoffersen/robotweb",
    project_urls={
        "Bug Tracker": "https://github.com/PChristoffersen/robotweb/issues",
    },
    classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: GNU General Public License v3 (GPLv3)",
        "Operating System :: Linux",
    ],
    
    package_dir={"": "src"},
    packages=find_packages(where="src"),
    
    include_package_data=True,
    package_data={
        "robotweb": [ "data/*", *glob_fix('robotweb', "www/**/*")]
    },

    entry_points={
        "console_scripts": [
            "robotweb = robotweb.__main__:main"
        ]
    },

    #requires=[
        #"setproctitle",
        #"aiohttp",
        #"python-socketio",
        #"orjson",
    #],

    python_requires=">=3.6",
)
