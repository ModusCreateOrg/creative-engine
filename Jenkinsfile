#!/usr/bin/env groovy
pipeline {
    agent any

    stages {
        stage('Clean') {
            steps {
                sh 'git clean -fdx'
            }
        }
        stage('Checkout Libs') {
            steps {
                dir('creative-engine') {
                   git branch: 'master', url: 'git@github.com:ModusCreateOrg/creative-engine'
                }
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
