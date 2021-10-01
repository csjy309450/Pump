pipeline {
  agent any
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