#    Copyright 2017 Shawn Gilroy
#
#    This file is part of Discounting Model Selector, port to c++.
#
#    Discounting Model Selector is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, version 2 (or any later version).
#
#    Discounting Model Selector is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with Discounting Model Selector.  If not, see <http://www.gnu.org/licenses/gpl-2.0.html>.
#
#    ==================================================================================================
#
#    Copyright 2016 Chris Franck
#
#    This file is part of Discounting Model Selector.
#
#    Discounting Model Selector is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, version 2 (or any later version).
#
#    Discounting Model Selector is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with Discounting Model Selector.  If not, see <http://www.gnu.org/licenses/gpl-2.0.html>.
#
#
# Input is a data frame with $X for delay, $Y for indiff point, and $ses for session
#
# Arguments      value     purpose
# Noise          binary    include noise model? (0 = pass, 1 = run)
# Mazur          binary    include Mazur model? (0 = pass, 1 = run)
# Exponential    binary    include Exponential model? (0 = pass, 1 = run)
# Rachlin        binary    include Rachlin model? (0 = pass, 1 = run, 2 = bound s parameter)
# GreenMyerson   binary    include Green-Myerson model? (0 = pass, 1 = run)
# BD             binary    include quasi-hyperboloid (aka beta-delta) model? (0 = pass, 1 = run)
#
# Version history
# v001 adapted from latest carilion day poster session
# v002 is developmental code to include starting value grid search for each method
# v003 incorprates a starting value grid search and outputs the selected starting values for each model and session
# v004 is development code to choose most probable model and output parameters and ED50 for that model - currently puts most probable model out
# v005 Is a starting value debug demo now and should not be used in batch mode
# v006 replaces generic starting values with optimal grid values
# v007 adds the beta-delta model option to the algorithm
# v008 corrects a critical error in the computation of ED50 for the beta delta model
# v009 introduce optional bounding for Rachlin's hyperboloid model

BDS<-function(dat,Noise=1,Mazur=1,Exponential=1,Rachlin=1,GreenMyerson=1,BD=1){
  #Debug option
  #Noise=1;Mazur=1;Exponential=1;Rachlin=1;GreenMyerson=1
  #define values needed to make loop run

  X<-dat$X
  Y<-dat$Y

  nonvarry<-rep(0,1)
  startstore<-matrix(NA,1,8,dimnames= list(NULL,c('Mg.lnK','Mg.s','Rach.lnK','Rach.s','Mazur.lnK','exp.lnK','BD.beta','BD.delta')))

  noise.frame<-data.frame(noise.mean=rep(NA,1),noise.RMSE=rep(NA,1),noise.BIC=rep(NA,1),noise.AIC=rep(NA,1),
                          noise.BF=rep(NA,1),noise.prob=rep(NA,1))

  Mazur.frame<-data.frame(Mazur.lnk=rep(NA,1),Mazur.RMSE=rep(NA,1),Mazur.BIC=rep(NA,1),Mazur.AIC=rep(NA,1),
                          Mazur.BF=rep(NA,1),Mazur.prob=rep(NA,1))

  exp.frame<-data.frame(exp.lnk=rep(NA,1),exp.RMSE=rep(NA,1),exp.BIC=rep(NA,1),exp.AIC=rep(NA,1),
                        exp.BF=rep(NA,1),exp.prob=rep(NA,1))

  MG.frame<-data.frame(MG.lnk=rep(NA,1),MG.s=rep(NA,1),MG.RMSE=rep(NA,1),MG.BIC=rep(NA,1),MG.AIC=rep(NA,1),
                       MG.BF=rep(NA,1),MG.prob=rep(NA,1))

  Rachlin.frame<-data.frame(Rachlin.lnk=rep(NA,1),Rachlin.s=rep(NA,1),Rachlin.RMSE=rep(NA,1),Rachlin.BIC=rep(NA,1),Rachlin.AIC=rep(NA,1),
                            Rachlin.BF=rep(NA,1),Rachlin.prob=rep(NA,1))

  BD.frame<-data.frame(BD.beta=rep(NA,1),BD.delta=rep(NA,1),BD.RMSE=rep(NA,1),BD.BIC=rep(NA,1),BD.AIC=rep(NA,1),
                       BD.BF=rep(NA,1),BD.prob=rep(NA,1))

  x<-X
  y<-Y

  nonvarry<-1

  #define the starting values of each parameter in the grid, get lengths
  startlnK<-seq(-12,12,1)
  starts<-seq(.01,10,.01)
  startbeta<-seq(0,1,.1)
  startdelta<-seq(0,1,.00011)
  LlnK<-length(startlnK)
  Ls<-length(starts)
  Lbeta<-length(startbeta)
  Ldelta<-length(startdelta)
  LX<-length(x)

  #Create the grid with both delay and indifference point values - needed to compute Squared deviations
  #S prefix denotes "Starting values" object for two parameter models
  SlnK<-rep(sort(rep(startlnK,LX)),Ls) #starting lnks
  Ss<-sort(rep(starts,LX*LlnK)) #starting s
  Sbeta<-rep(sort(rep(startbeta,LX)),Ldelta) #starting lnks
  Sdelta<-sort(rep(startdelta,LX*Lbeta)) #starting s

  SX<-rep(x,Ls*LlnK) #Delays in test case
  SY<-rep(y,Ls*LlnK) #indifference points in test case
  SY_BD<-rep(y,Lbeta*Ldelta) #indifference points in test case)

  Sind   <-sort(rep(1:(Ls*LlnK),LX)) #indexes the pairs of starting values
  Sind_BD<-sort(rep(1:(Lbeta*Ldelta),LX)) #indexes the pairs of starting values

  SMGpred<-(1+exp(SlnK)*x)^(-Ss) #MG predictions
  SRachpred<-(1+exp(SlnK)*(x^Ss))^(-1)

  SBDpred<-Sbeta*Sdelta^x  #BD predictions

  SMGsquare<-(SY-SMGpred)^2 #Squared deviations for MG
  SRachsquare<-(SY-SRachpred)^2 #Squared deviations for MG
  SBDsquare<-(SY_BD-SBDpred)^2 #Squared deviations for BD

  #M prefix denotes starting values for one parameter model Mazur Samuelson
  MlnK<-sort(rep(startlnK,LX))
  MX<-rep(x,LlnK)
  MY<-rep(y,LlnK)
  Mind<-sort(rep(1:(LlnK),LX))
  MMazurpred<-(1+exp(MlnK)*MX)^(-1)
  MMazursquare<-(MY-MMazurpred)^2
  Mexppred<-exp(-exp(MlnK)*MX)
  Mexpsquare<-(MY-Mexppred)^2

  #Compute sum of squares and associate it with the starting values
  #SS prefix denotes objects used to determine best starting values
  SSlnK<-rep(startlnK,Ls)
  SSs<-sort(rep(starts,LlnK))
  SSMGss<-rep(NA,Ls*LlnK)
  SSRachss<-rep(NA,Ls*LlnK)
  SSbeta<-rep(startbeta,Ldelta)  #why do I need this? I dont see an analog for other models?
  SSdelta<-sort(rep(startdelta,Lbeta))  #why do I need this? I dont see an analog for other models?
  SSBDss<-rep(NA,Lbeta*Ldelta)


  for(j in 1:(Ls*LlnK)){
    SSMGss[j]<-sum(SMGsquare[(j-1)*LX +1:length(x)])
    SSRachss[j]<-sum(SRachsquare[(j-1)*LX +1:length(x)])
  }
  for(j in 1:(Lbeta*Ldelta)){
    SSBDss[j]<-sum(SBDsquare[(j-1)*LX +1:length(x)])
  }

  SSMazur<-rep(NA,LlnK)
  SSexp<-rep(NA,LlnK)
  #Samuelson and Mazur loop would go here
  for(j in 1:LlnK){
    SSMazur[j]<-sum(MMazursquare[(j-1)*LX +1:length(x)])
    SSexp[j]<-sum(Mexpsquare[(j-1)*LX +1:length(x)])
  }

  presort.MG<-data.frame(SSlnK,SSs,SSMGss)
  presort.Rach<-data.frame(SSlnK,SSs,SSRachss)
  presort.Mazur<-data.frame(startlnK,SSMazur)
  presort.exp<-data.frame(startlnK,SSexp)
  presort.BD<-data.frame(SSbeta,SSdelta,SSBDss)

  sorted.MG<-presort.MG[order(presort.MG[,3]),]
  sorted.Rach<-presort.Rach[order(presort.Rach[,3]),]
  sorted.Mazur<-presort.Mazur[order(presort.Mazur[,2]),]
  sorted.exp<-presort.exp[order(presort.exp[,2]),]
  sorted.BD<-presort.BD[order(presort.BD[,3]),]

  ini.par.MG <- c(lnK=sorted.MG$SSlnK[1],s=sorted.MG$SSs[1])
  ini.par.Rach <- c(lnK=sorted.Rach$SSlnK[1],s=sorted.Rach$SSs[1])
  ini.par.Mazur<-c(lnK=sorted.Mazur$startlnK[1])
  ini.par.exp<-c(lnK=sorted.exp$startlnK[1])
  ini.par.BD <- c(beta=sorted.BD$SSbeta[1],delta=sorted.BD$SSdelta[1])

  startstore<-c(Mg=ini.par.MG,Rach=ini.par.Rach,Mazur=ini.par.Mazur,exp=ini.par.exp,BD=ini.par.BD)

  #Stores which models are competitors for this run
  AF<-c(Noise, Mazur,Exponential,GreenMyerson,Rachlin,BD)

  #Fit mean + noise model
  if((Noise==1)&(!is.character(try(lm(y~1),silent=TRUE)))){
    modfit.noise<-lm(y~1)
    noise.frame$noise.mean<-summary(modfit.noise)$coeff[1,1]
    noise.frame$noise.RMSE<-summary(modfit.noise)$sigma
    noise.frame$noise.BIC<-BIC(modfit.noise)
    noise.frame$noise.AIC<-AIC(modfit.noise)
  }else(AF[1]<-0)

  #fit Mazur model (1987) - single parameter hyperbolic function
  ini.par <- ini.par.Mazur #Starting points
  #if(Mazur==1){
  if((Mazur==1)&( !is.character(try(nls(y~(1+exp(lnK)*x)^(-1), start = ini.par),silent=TRUE))    )){
    modfit.Mazur<-nls(y~(1+exp(lnK)*x)^(-1), start = ini.par)
    Mazur.frame$Mazur.lnk<-summary(modfit.Mazur)$coeff[1,1]
    Mazur.frame$Mazur.RMSE<-summary(modfit.Mazur)$sigma
    Mazur.frame$Mazur.BIC<-BIC(modfit.Mazur)
    Mazur.frame$Mazur.AIC<-AIC(modfit.Mazur)
  }else(AF[2]<-0)

  #fit (Samuelson, 1937), - single parameter expopnential
  ini.par <- ini.par.exp #Starting points
  if((Exponential==1)&(!is.character(try(nls(y~exp(-exp(lnK)*x), start = ini.par),silent=TRUE)))){
    modfit.exp<-nls(y~exp(-exp(lnK)*x), start = ini.par)
    exp.frame$exp.lnk<-summary(modfit.exp)$coeff[1,1]
    exp.frame$exp.RMSE<-summary(modfit.exp)$sigma
    exp.frame$exp.BIC<-BIC(modfit.exp)
    exp.frame$exp.AIC<-AIC(modfit.exp)
  }else(AF[3]<-0)

  #Fit the Myerson and Green model - two parameter hyperbolic function
  ini.par <- ini.par.MG #Starting points
  if((GreenMyerson==1)&(!is.character(try(nls(y~(1+exp(lnK)*x)^(-s), start = ini.par),silent=TRUE)))){
    modfit.MG<-try(nls(y~(1+exp(lnK)*x)^(-s), start = ini.par),silent=TRUE)
    MG.frame$MG.lnk<-summary(modfit.MG)$coeff[1,1]
    MG.frame$MG.s<-summary(modfit.MG)$coeff[2,1]
    MG.frame$MG.RMSE<-summary(modfit.MG)$sigma
    MG.frame$MG.BIC<-BIC(modfit.MG)
    MG.frame$MG.AIC<-AIC(modfit.MG)
  }else(AF[4]<-0)

  #Fit the Rachlin model
  ini.par <- ini.par.Rach #Starting points

  if((Rachlin==1)&(!is.character(try(nls(y~(1+exp(lnK)*(x^s))^(-1), start = ini.par), silent=FALSE)))){
    modfit.Rachlin<-nls(y~(1+exp(lnK)*(x^s))^(-1), start = ini.par)
    Rachlin.frame$Rachlin.lnk<-summary(modfit.Rachlin)$coeff[1,1]
    Rachlin.frame$Rachlin.s<-summary(modfit.Rachlin)$coeff[2,1]
    Rachlin.frame$Rachlin.RMSE<-summary(modfit.Rachlin)$sigma
    Rachlin.frame$Rachlin.BIC<-BIC(modfit.Rachlin)
    Rachlin.frame$Rachlin.AIC<-AIC(modfit.Rachlin)
  } else(AF[5]<-0)

  ini.par<-ini.par.BD
  if((BD==1)&(!is.character(try( nls(y~beta*delta^x,start=ini.par,algorithm='port',lower=0,upper=1),silent=TRUE)))){
    modfit.BD<-nls(y~beta*delta^x,start=ini.par,algorithm='port',lower=0,upper=1)
    BD.frame$BD.beta<-summary(modfit.BD)$coeff[1,1]
    BD.frame$BD.delta<-summary(modfit.BD)$coeff[2,1]
    BD.frame$BD.RMSE<-summary(modfit.BD)$sigma
    BD.frame$BD.BIC<-BIC(modfit.BD)
    BD.frame$BD.AIC<-AIC(modfit.BD)
  }else(AF[6]<-0)

  ###Compute model probabilities
  BICvec<-c(noise.frame$noise.BIC,Mazur.frame$Mazur.BIC,exp.frame$exp.BIC,MG.frame$MG.BIC,Rachlin.frame$Rachlin.BIC,BD.frame$BD.BIC)
  BICstore<-BICvec[!is.na(BICvec)]
  BFvec<-exp(-.5*(BICstore-BICstore[1]))
  probvec<-BFvec/sum(BFvec)

  #assign probs and BFs to the appropriate model

  mprobvec<-rep(NA,6) #initialize
  mBFvec<-rep(NA,6)
  mi<-1
  for(j in 1:6){
    if(AF[j]==1 || AF[j]==2){mprobvec[j]<-probvec[mi];mBFvec[j]<-BFvec[mi];mi<-mi+1}
    if(AF[j]==0){mprobvec[j]<-NA;mBFvec[j]<-NA}
  }

  noise.frame$noise.BF<-mBFvec[1]
  noise.frame$noise.prob<-mprobvec[1]
  Mazur.frame$Mazur.BF<-mBFvec[2]
  Mazur.frame$Mazur.prob<-mprobvec[2]
  exp.frame$exp.BF<-mBFvec[3]
  exp.frame$exp.prob<-mprobvec[3]
  MG.frame$MG.BF<-mBFvec[4]
  MG.frame$MG.prob<-mprobvec[4]
  Rachlin.frame$Rachlin.BF<-mBFvec[5]
  Rachlin.frame$Rachlin.prob<-mprobvec[5]

  BD.frame$BD.BF<-mBFvec[6]
  BD.frame$BD.prob<-mprobvec[6]

  prob.frame<-data.frame(
    Noise=noise.frame$noise.prob,
    Mazur=Mazur.frame$Mazur.prob,
    Exponential=exp.frame$exp.prob,
    GreenMyerson=MG.frame$MG.prob,
    Rachlin=Rachlin.frame$Rachlin.prob,
    BD=BD.frame$BD.prob,
    nonvarry
  )

  #Call the most probable model

  findmax<-prob.frame[,1:6]
  findmax[is.na(findmax)]<-0

  themax<-apply(findmax,1,max)
  markmax<-matrix(NA,dim(findmax)[1],dim(findmax)[2],dimnames=list(NULL,c("Noise","Mazur","Exponential","GreenMyerson","Rachlin","BD")))
  namemax<-rep(NA,dim(findmax)[1])
  for(k in 1:dim(findmax)[1]){
    markmax[k,]<-as.numeric(findmax[k,]==themax[k])
    namemax[k]<-colnames(markmax)[markmax[k,]==1]
  }

  prob.frame<-data.frame(prob.frame,probmodel=namemax)

  #Compute Mazure and most probable ED50
  #Incorporate into BDS
  lnED50.Mazur<-log(1/(exp(Mazur.frame$Mazur.lnk)))

  lnED50.mostprob<-rep(NA,dim(prob.frame)[1])
  for(h in 1:dim(prob.frame)[1]){
    if(prob.frame$probmodel[h]=="Mazur"){lnED50.mostprob[h]<-log(1/(exp(Mazur.frame$Mazur.lnk)))[h]}
    if(prob.frame$probmodel[h]=="Exponential"){lnED50.mostprob[h]<-log(log(2)/exp(exp.frame$exp.lnk)[h])}
    if(prob.frame$probmodel[h]=="GreenMyerson"){lnED50.mostprob[h]<-log( (2^(1/MG.frame$MG.s[h])-1)/exp(MG.frame$MG.lnk[h])  )}
    if(prob.frame$probmodel[h]=="Rachlin"){lnED50.mostprob[h]<-log( (1/(exp(Rachlin.frame$Rachlin.lnk)[h]))^(1/Rachlin.frame$Rachlin.s[h]) )}
    if(prob.frame$probmodel[h]=="BD"){lnED50.mostprob[h]<- log(log( (1/(2*BD.frame$BD.beta[h])),base=BD.frame$BD.delta[h]  ))}
  }

  integrandExp <- function(x, lnK) { exp(-exp(lnK)*x) }
  integrandHyp <- function(x, lnK) { (1+exp(lnK)*x)^(-1) }
  integrandMyerson <- function(x, lnK, s) { (1+exp(lnK)*x)^(-s) }
  integrandRachlin <- function(x, lnK, s) { (1+exp(lnK)*(x^s))^(-1) }
  integrandBetaDelta <- function(x, beta, delta) { beta*delta^x }

  ### AUC Here
  currentArea <- 0.0

  mData <- data.frame(X=x, Y=y)
  maximumArea <- max(mData$X)

  #start one leg in
  for(i in 2:nrow(mData)) {
    yDelta <- (mData[i,]$Y + mData[i-1,]$Y)/2.0
    xDelta <- mData[i,]$X - mData[i-1,]$X

    trap <- yDelta * xDelta
    currentArea = currentArea + trap
  }

  maximumArea <- max(mData$X) - min(mData$Y)

  AUC <- currentArea/maximumArea

  AUK <- NULL

  if (prob.frame$probmodel == "Rachlin") {
    AUK <- integrate(integrandRachlin, lower = min(mData$Y), upper = maximumArea, lnK = Rachlin.frame$Rachlin.lnk, s = Rachlin.frame$Rachlin.s)$value/maximumArea

  } else if (prob.frame$probmodel == "Exponential") {
    AUK <- integrate(integrandExp, lower = min(mData$Y), upper = maximumArea, lnK = exp.frame$exp.lnk)$value/maximumArea

  } else if (prob.frame$probmodel == "Mazur") {
    AUK <- integrate(integrandHyp, lower = min(mData$Y), upper = maximumArea, lnK = Mazur.frame$Mazur.lnk)$value/maximumArea

  } else if (prob.frame$probmodel == "GreenMyerson") {
    AUK <- integrate(integrandMyerson, lower = min(mData$Y), upper = maximumArea, lnK = MG.frame$MG.lnk, s = MG.frame$MG.s)$value/maximumArea

  } else if (prob.frame$probmodel == "BD") {
    AUK <- integrate(integrandBetaDelta, lower = min(mData$Y), upper = maximumArea, beta = BD.frame$BD.beta, delta = BD.frame$BD.delta)$value/maximumArea

  } else {
    AUK <- noise.frame$noise.mean
  }

  lnED50res<-data.frame(prob.frame$probmodel,lnED50.mostprob,lnED50.Mazur,Mazur.frame$Mazur.lnk)

  chartFrame<-data.frame(chart=chartFunction(X, Y, exp.frame$exp.lnk,Mazur.frame$Mazur.lnk,
                                             BD.frame$BD.beta,BD.frame$BD.delta,
                                             MG.frame$MG.lnk,MG.frame$MG.s,
                                             Rachlin.frame$Rachlin.lnk,Rachlin.frame$Rachlin.s,
                                             noise.frame$noise.mean,prob.frame$probmodel,
                                             AUC,AUK), PointArea=AUC, CurveArea=AUK)

  return(list(noise.frame,Mazur.frame,exp.frame,MG.frame,Rachlin.frame,prob.frame,startstore,lnED50res,BD.frame, chartFrame))
}

# Create a plot comparing area-metrics, using results of BDS
#
# Shawn P. Gilroy, 2017 (aut)
chartFunction<-function(mDelays, mValues, samuelsonK=0,ainslieK=0,betaConstant=0,deltaConstant=0,
                        myerK=0,myerS=0,rachK=0,rachS=0,noiseMean=0,probModel=0,
                        AUC=0,AUK=0){

  endDelay <- max(mDelays)

  sink(tempfile())

  options(warn=-1)

  if(!require(reshape)){
    install.packages("reshape")
    if(!require(reshape)){
      install.packages("reshape")
      library(reshape)
    }
  }

  if(!require(base64enc)){
    install.packages("base64enc")
    if(!require(base64enc)){
      install.packages("base64enc")
      library(base64enc)
    }
  }

  if(!require(jsonlite)){
    install.packages("jsonlite")
    if(!require(jsonlite)){
      install.packages("jsonlite")
      library(jsonlite)
    }
  }

  sink()

  returnFrame <- data.frame(lineChart="")

  delaySeries = 1:(endDelay+1)
  expSeries = rep(NA,endDelay+1)
  hypSeries = rep(NA,endDelay+1)
  quaSeries = rep(NA,endDelay+1)
  myerSeries = rep(NA,endDelay+1)
  rachSeries = rep(NA,endDelay+1)

  for (delay in delaySeries)
  {
    delaySeries[delay] = delay-1

    if(!is.na(samuelsonK))
    {
      expSeries[delay] = (1.0 * exp(-(exp(samuelsonK))*delay))
    }
    if(!is.na(ainslieK))
    {
      hypSeries[delay] = 1.0 * (1+exp(ainslieK)*delay)^(-1)
    }
    if(!is.na(betaConstant))
    {
      quaSeries[delay] = 1.0 * ((betaConstant)*(deltaConstant)^delay)
    }
    if(!is.na(myerK))
    {
      myerSeries[delay] = 1.0 * (1+exp(myerK)*delay)^(-myerS)
    }
    if(!is.na(rachK))
    {
      rachSeries[delay] = 1.0 * (1 + exp(rachK)*(delay^rachS))^(-1)
    }
  }

  totalFrame = data.frame(Delays=delaySeries)

  if (probModel == "Rachlin") {
    totalFrame$RachlinModel = rachSeries

  } else if (probModel == "Exponential") {
    totalFrame$ExponentialModel = expSeries

  } else if (probModel == "Mazur") {
    totalFrame$HyperbolicModel = hypSeries

  } else if (probModel == "GreenMyerson") {
    totalFrame$MyersonGreenModel = myerSeries

  } else if (probModel == "BD") {
    totalFrame$QuasiHyperbolic = quaSeries

  } else {
    totalFrame$NoiseModel = mean(mValues)

  }

  textModel = paste("Probable model: ", probModel, sep = "")
  textAUC = paste("Empirical Discounting Function AUC:", AUC, sep = " ")
  textAUK = paste("Numerical Integration Function AUC:", AUK, sep = " ")

  totalFrame.melt <- suppressMessages(reshape::melt(totalFrame, id=c("Delays")))

  pointFrame <- data.frame(DelaysP=mDelays, ValuesP=mValues)

  svg(tempf2 <- tempfile(fileext = '.svg'))

  plot(totalFrame.melt$Delays, totalFrame.melt$value, type = "l", ylim = c(0,1),
       main = paste("Area Under Curve (", probModel, ")", sep = ""),
       xlab = "Delays", ylab = "Values", col = "red")
  points(pointFrame$DelaysP, pointFrame$ValuesP, type = "b")

  legend('bottomleft', c(textAUC, textAUK),
         lty=1, col=c('black', 'red'), bty='n', cex=.75)

  dev.off()

  lineString <- base64enc::base64encode(readBin(tempf2, 'raw', file.info(tempf2)[1, 'size']), 'txt')

  unlink(tempf2)

  return(lineString)
}

myArgs <- commandArgs(trailingOnly = TRUE)

mDelays <- unlist(strsplit(myArgs[1], split=","))
mValues <- unlist(strsplit(myArgs[2], split=","))
mModels <- as.numeric(unlist(strsplit(myArgs[3], split=",")))

dat <- data.frame(X=as.numeric(mDelays), Y=as.numeric(mValues))
dat$ses = 1

result = BDS(dat,mModels[1],mModels[2],mModels[3],mModels[4],mModels[5],mModels[6])

printin <- toJSON(reshape::merge_recurse(result)[1,], pretty = FALSE)

printin
