from setuptools import setup, find_packages

setup(
    name="snek",
    version="0.1",
    packages=find_packages(),
    install_requires=["pygame>=2.0.0"],
    entry_points={
        'console_scripts': [
            'snek=snek.main:main',
        ],
    },
)