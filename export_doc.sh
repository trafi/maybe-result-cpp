#!/usr/bin/env bash

if [ "$TRAVIS_BRANCH" != 'master' ]; then
    echo "skipping doc export because branch is not master"
    exit;
fi

if [ "$DOC" != "true" ]; then
    echo "skipping doc export because not enabled for this build"
    exit;
fi

echo "exporting docs"

export PATH=$HOME/.local/bin:$PATH
sudo apt-add-repository -y ppa:kedazo/doxygen-updates-precise
sudo apt-get update
sudo apt-get install -y doxygen
doxygen
pip install ghp-import --user `whoami`
ghp-import -n html
git push -qf https://${TOKEN}@github.com/${TRAVIS_REPO_SLUG}.git gh-pages
