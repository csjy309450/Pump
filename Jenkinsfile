pipeline {
  agent {
    node {
      label 'Yz_JenkinsNode0_CentOS7_x64_0.104'
    }

  }
  stages {
    stage('build') {
      steps {
        sh '''mkdir build
cd build
cmake ../
make'''
      }
    }

  }
}