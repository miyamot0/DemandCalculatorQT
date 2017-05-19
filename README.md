# Small N Stats Demand Curve Analyzer (Qt)

Small N Stats Demand Curve Analyzer is a Qt-based application that communicates with R to easily facilitate complex calculations required of behavior economic (BE) scientists.

Features include:
  - Non-linear model fittings (John Nash's modified LM optimizer, specificially suited for BE models)
  - Multiple Models available (Hursh & Silberberg's Exponential Model, Koffarnus et. al.'s Exponentiated model)
  - R-based graphical output in appropriate log space(s)
  - Easily imports common file formats into the application's spreadsheet (.xlsx)
  - Full range of BE outcome metrics (empirical and derived), saveable in common spreadsheet file formats

### Version
1.0.0.0

### Changelog
 * 1.0.0.0 - alpha build

### Referenced Works (F/OSS software)
The Small N Stats Demand Calculator uses a number of open source projects to work properly:
* R Statistical Package - GPLv2+ (GPL 3.0 Licensed). Copyright (C) 2000-17. The R Core Team [Site](https://www.r-project.org/)
* Qt Framework (5.8) - LGPL 3.0 (GPL 3.0 Licensed). Copyright (C). Qt Team [Site](https://www.qt.io/)
* Qt Examples (Spreadsheet, Recent Files) - BSD-3 Clause. Copyright (C). Qt Team [Site](https://www.qt.io/)
* Gnome icons - GPLv3 Licensed. Copyright (C) "GNOME Project" [Site](http://www.gnome.org)

### Referenced Works (R packages/scripts)
* beezdemand R package - GPLv2+ Licensed. Copyright (c) 2016, Brent Kaplan [Github](https://github.com/brentkaplan/beezdemand)
* nlmrt R Package - GPLv2+ Licensed. Copyright (C) 2016. John C. Nash
* devtools R package - GPLv2+ Licensed. Copyright (c) 2016, Hadley Wickham, Winston Chang, R Core Team [Github](https://github.com/hadley/devtools)
* digest R package - GPLv2+ Licensed. Copyright (c) 2016, Dirk Eddelbuettel with contributions by Antoine Lucas, Jarek Tuszynski, Henrik Bengtsson, Simon Urbanek, Mario Frasca, Bryan Lewis, Murray Stokely, Hannes Muehleisen, Duncan Murdoch, Jim Hester, Wush Wu, Qiang Kou, Thierry Onkelinx, Michel Lang and Viliam Simko [Github](https://github.com/eddelbuettel/digest)
* nlstools R Package - GPLv2+ Licensed. Copyright(C) 2015 Florent Baty and Marie-Laure Delignette - Muller, with contributions from Sandrine Charles, Jean - Pierre Flandrois, and Christian Ritz
* nls R Package (stats) - GPLv2+ (GPL 3.0 Licensed). Copyright (C) 1999-1999 Saikat DebRoy, Douglas M. Bates, Jose C. Pinheiro.
* nls R Package (stats) - GPLv2+ (GPL 3.0 Licensed). Copyright (C) 2000-17. The R Core Team.
* base64enc R Package - GPLv2+ (GPL 3.0 Licensed). Copyright (c) 2015, Simon Urbanek
* jsonlite R Package - MIT Licensed. Copyright (c) 2017, Jeroen Ooms.

### Referenced Works (academic works)
The Small N Stats Demand Calculator is based on the following academic works:
* Hursh, S. R. and Silberberg, A. (2008). Economic demand and essential value. Psychological Review, 115, 186?198.
* Koffarnus, M. N., Franck, C. T., Stein, J. and Bickel, W. K. (2015). A modified exponential behavioral economic demand model to better describe consumption data. Experimental Clinical Psychopharmacology, 23, 504-512.

### Acknowledgements and Credits
* Brent Kaplan, Applied Behavioral Economics Laboratory, University of Kansas (www.behavioraleconlab.com) [Github](https://github.com/brentkaplan)
* Derek D. Reed, Applied Behavioral Economics Laboratory, University of Kansas (www.behavioraleconlab.com) [Github](https://github.com/derekdreed)
* Donald A. Hantula, Decision Making Laboratory, Temple University [Site](http://astro.temple.edu/~hantula/)
* Chris Franck, Laboratory for Interdisciplinary Statistical Analysis - Virginia Tech

### Installation
You will need the R open-source statistical package for model fitting/charting to be performed.
Once DemandCalculator is installed, it will perform a one-time install the necessary R packages (internet required).
DemandCalculator is a ClickOnce application, the program will automatically update as the program is refined.

### Download
All downloads, if/when posted, will be hosted at [Small N Stats](http://www.smallnstats.com/DemandAnalysis.html).

### Development
Want to contribute? Great! Emails or PM's are welcome.

### Todos
* Beta testing

### License
----
Demand Curve Analyzer (Qt) - Copyright 2017, Shawn P. Gilroy. GPL-Version 3
