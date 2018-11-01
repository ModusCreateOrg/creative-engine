#!/usr/bin/env groovy
pipeline {
    agent any

    stages {
        stage('Clean') {
            steps {
                sh 'git clean -fdx'
            }
        }
        stage('Build') {
            steps {
                sh '''
                   OS="$(uname)"
                   if [ "$OS" == "Darwin" ]; then
                      scripts/build.sh
                   else
                      scripts/docker-build.sh
                   fi
                   '''
            }
        }
    }
}
