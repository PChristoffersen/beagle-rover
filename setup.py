from importlib.metadata import entry_points
from setuptools import setup, find_packages
from pathlib import Path

with open("README.md", "r", encoding="utf-8") as fh:
    long_description = fh.read()

def glob_fix(package_name, glob):
    # this assumes setup.py lives in the folder that contains the package
    package_path = Path(f'./src/{package_name}').resolve()
    return [str(path.relative_to(package_path)) 
            for path in package_path.glob(glob)]

setup(
    name="robotweb",
    version="0.0.1",
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

    requires=[
        #"setproctitle",
        #"aiohttp",
        #"python-socketio",
        #"orjson",
    ]

    python_requires=">=3.6",
)
