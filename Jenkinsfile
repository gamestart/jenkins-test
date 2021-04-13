#!/usr/bin/env groovy

pipeline{
    agent none
    stages {
        stage('Parallel Stage') {
            failFast true
            parallel {
		stage('win10'){
	    		agent {
	       			label 'win10'
		   		 }
			    stages {
			       stage("GetPackage"){  
			            steps{    
			                script{  
			                   powershell '''
			                      $package_name="package-win"
			                      $wc=New-Object net.webclient
			                      $url='http://192.168.100.12/artifactory/smore_vimo-runtime-lib/package-win.zip'
			                      $package_destination_path = "$pwd/$package_name.zip"
			                      $wc.Downloadfile( $url, $package_destination_path)
			                      Expand-Archive -Path "$package_name.zip"
			                      Remove-Item  "$pwd/lib"  -Recurse
			                      Remove-Item  "$pwd/include"  -Recurse
			                      Copy-Item "$pwd/$package_name/*"  -Destination "$pwd" -Recurse
			                      Remove-Item  "$pwd/$package_name*"  -Recurse
			                   '''
			                }//script
			            }//steps
			        }//stage
			
		        
	         	    	}//stages
         		}//stage
		stage('Pwin10'){
                        agent {
                                label 'Pwin10'
                                 }
                            stages {
                               stage("GetPackage"){
                                    steps{
                                        script{
                                           powershell '''
                                              $package_name="package-win"
                                              $wc=New-Object net.webclient
                                              $url='http://192.168.100.12/artifactory/smore_vimo-runtime-lib/package-win.zip'
                                              $package_destination_path = "$pwd/$package_name.zip"
                                              $wc.Downloadfile( $url, $package_destination_path)
                                              Expand-Archive -Path "$package_name.zip"
                                              Remove-Item  "$pwd/lib"  -Recurse
                                              Remove-Item  "$pwd/include"  -Recurse
                                              Copy-Item "$pwd/$package_name/*"  -Destination "$pwd" -Recurse
                                              Remove-Item  "$pwd/$package_name*"  -Recurse
                                           '''
                                        }//script
                                    }//steps
                                }//stage


                                }//stages
                        }//stage
		}//parallel
	}//stage
    }//stages
}//pipeline
