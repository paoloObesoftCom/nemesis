// $Header$
//Helper functions
Element.prototype.fadeIn = function(parms, out) {
	var _this = this;
	var opacityTo = function(elm,v){
		elm.style.opacity = v/100;
		elm.style.MozOpacity =  v/100;
		elm.style.KhtmlOpacity =  v/100;
		elm.style.filter=" alpha(opacity ="+v+")";
	};
	var delay = parms.delay;
	var to = parms.to;
	if(!to)
		to = 100;
	
	_this.style.zoom = 1;
	// for ie, set haslayout
	_this.style.display = "block";

	for (var i=1; i<=to; i++) {
		(function(j) {
			setTimeout(function() {
				if (out == true)
					j = to - j;
				opacityTo(_this, j);
			}, j * delay / to);
		})(i);
	};
};
Element.prototype.fadeOut = function(delay) {
	this.fadeIn({'delay' : delay}, true);
};

String.prototype.e = function(){
return this.replace("\"","&quot;");
};

//General Helpers
function toOptionList(lst, selected, split) {
	var retList = Array();
	retList.push("");
	if (split && !selected == '') {
		selected = selected.split(split);
	} else {
		if (selected && selected != "") {
			selected = [ selected ];
		} else {
			selected = [];
		}
	}

	selected.each(function(item) {
		var found = false;
		lst.each(function(listItem) {

			if (listItem == item) {
				found = true;
			}
		});
		if (!found) {
			lst.push(item);
		}
	});

	lst.each(function(listItem) {
		var sel = '';
		selected.each(function(item) {
			if (listItem == item) {
				sel = 'selected';
			}
		});

		retList.push({
			'value' : listItem,
			'txt' : listItem,
			'selected' : sel
		});
	});

	return retList;
}

function debug(item) {
	if (userprefs.data.debug)
		console.log(item);
}

function parseNr(num) {
	if (isNaN(num)) {
		return 0;
	} else {
		return parseInt(num);
	}
}

function getNodeContent(xml, nodename, defaultString){
	try{
		var node = xml.getElementsByTagName(nodename);
		var retVal = node.item(0).firstChild.data;

		if(retVal === "" || retVal === null){
			return 'N/A';		
		} else if (retVal === "None"){
			return "";
		}
		
		return retVal;
	} catch(e){
		if(typeof(defaultString) !== 'undefined') {
			return defaultString;
		}		
	}
	
	return 'N/A';
}

function getNamedChildren(xml, parentname, childname){
	try {
		var ret = xml.getElementsByTagName(parentname).item(0).getElementsByTagName(childname);
		return ret;
	} catch (e) {
		return {};
	}
}


function dec2hex(nr, len){
	var hex = parseInt(nr, 10).toString(16).toUpperCase();
	if(len > 0){
		try{
			while(hex.length < len){
				hex = "0"+hex;
			}
		} 
		catch(e){
			//something went wrong, return -1
			hex = -1;
		}
	}
	hex = '0x' + hex;
	return hex;
}


function quotes2html(txt) {
	if(txt !== undefined){
		return txt.escapeHTML().replace('\n', '<br>');
	} else {
		return "";
	}
}

function addLeadingZero(nr){
	if(nr < 10){
		return '0' + nr;
	}
	return nr;
}

function repeatedReadable(num) {
	num = Number(num);
	if (num === 0) {
		return "One Time";
	}

	var retVal = "";
	var Repeated = {};
	Repeated["Mo-Su"] = 127;
	Repeated["Mo-Fr"] = 31;
	Repeated["Su"] = 64;
	Repeated["Sa"] = 32;
	Repeated["Fr"] = 16;
	Repeated["Th"] = 8;
	Repeated["We"] = 4;
	Repeated["Tu"] = 2;
	Repeated["Mo"] = 1;

	for (var rep in Repeated) {
		if (rep.toString() != 'extend') {
			var check = Number(Repeated[rep]);
			if (!(~num & check)) {
				num -= check;
				if (retVal === '') {
					retVal += rep.toString();
				} else {
					retVal = rep.toString() + ',' + retVal;
				}
			}
		}
	}
	return retVal;
}

function dateToString(date){
	var dateString = "";
	dateString += date.getFullYear();
	dateString += "-" + addLeadingZero(date.getMonth()+1);
	dateString += "-" + addLeadingZero(date.getDate());
	dateString += " " + addLeadingZero(date.getHours());
	dateString += ":" + addLeadingZero(date.getMinutes());
	return dateString;
}

// store all objects here

var AjaxThing = Class.create({
	/**
	 * getUrl
	 * creates a new Ajax.Request
	 * Parameters:
	 * @url - the url to fetch
	 * @parms - an json object containing  {parameter : value} pairs for the request
	 * @callback - function to call @ onSuccess;
	 * @errorback - function to call @ onError;
	 */
	getUrl: function(url, parms, callback, errorback){
		debug("[AjaxThing].getUrl :: url=" + url + " :: parms=" + Object.toJSON(parms));
		try{
			new Ajax.Request(url,
					{
						parameters: parms,
						asynchronous: true,
						method: 'POST',
						requestHeaders: ['Cache-Control', 'no-cache,no-store', 'Expires', '-1'],
						onException: function(o,e){ 
								console.log(o); 
								console.log(e);
								throw(e);
							}.bind(this),
						onSuccess: function (transport, json) {
							if(callback !== undefined){
								callback(transport);
							}
						}.bind(this),
						onFailure: function(transport){
							if(errorback !== undefined){
								errorback(transport);
							}
						}.bind(this)
					});
		} catch(e) {
			debug('[AbstractContentProvider.getUrl] Exception: '+ e);
		}
	}
});


var TemplateEngine = Class.create(AjaxThing, {
	initialize: function(){
		this.templates = {};
	},

	cache: function(request, tplName){
		debug("[TemplateEngine].cache caching template: " + tplName);
		this.templates[tplName] = request.responseText;
	},
	
	fetch: function(tplName, callback){
		if(this.templates[tplName] === undefined) {
			var url = URL.tpl+tplName+".htm";
			
			this.getUrl(
					url, 
					{},
					function(transport){
						this.cache(transport, tplName);
						if(typeof(callback) == 'function'){
							callback(this.templates[tplName]);
						}
					}.bind(this)
			);
		} else {
			if(typeof(callback) == 'function'){
				callback(this.templates[tplName]);
			}
		}
	},
	
	render: function(tpl, data, domElement) {	
		var result = tpl.process(data);
	
		try{
			$(domElement).update( result );
		}catch(ex){
			debug("[TemplateEngine].render catched an exception!");
			throw ex;
		}
	},
	
	onTemplateReady: function(tpl, data, domElement, callback){
		this.render(tpl, data, domElement);
		if(typeof(callback) == 'function') {
			callback();
		}
	},
	
	process: function(tplName, data, domElement, callback){
		this.fetch( tplName, 
				function(tpl){
					this.onTemplateReady(tpl, data, domElement, callback); 
				}.bind(this) );
	}
});
templateEngine = new TemplateEngine();

//START class EPGEvent
function EPGEvent(xml, number){	
	this.eventID = getNodeContent(xml, 'e2eventid', '');
	this.startTime = parseNr(getNodeContent(xml, 'e2eventstart', ''));	
	this.duration = parseNr(getNodeContent(xml, 'e2eventduration', ''));
	this.currentTime = parseNr(getNodeContent(xml, 'e2eventcurrenttime')),
	this.title = getNodeContent(xml, 'e2eventtitle', '');
	this.serviceRef = getNodeContent(xml, 'e2eventservicereference', '');
	this.serviceName = getNodeContent(xml, 'e2eventservicename', '');
	this.fileName = getNodeContent(xml, 'e2filename', '');	
	this.description = getNodeContent(xml, 'e2eventdescription');
	this.descriptionE = getNodeContent(xml, 'e2eventdescriptionextended');
	
	if(typeof(number) != "undefined"){
		this.number = number;
	} else {
		this.number = 0;
	}
	
	this.getFilename = function() {
		return this.fileName;
	};
	this.getEventId = function() {
		return this.eventID;
	};
	this.getTimeStart = function() {
		var date = new Date(this.startTime *1000);
		return date;
	};
	this.getTimeStartString = function() {
		var h = this.getTimeStart().getHours();
		var m = this.getTimeStart().getMinutes();
		if (m < 10){
			m="0"+m;
		}
		return h+":"+m;
	};
	this.getTimeDay = function() {
		var weekday = ["So", "Mo", "Tu", "We", "Th", "Fr", "Sa"];
		var wday = weekday[this.getTimeStart().getDay()];
		var day = this.getTimeStart().getDate();
		var month = this.getTimeStart().getMonth()+1;
		var year = this.getTimeStart().getFullYear();
		
		return wday+".&nbsp;"+day+"."+month+"."+year;
	};
	this.getTimeBegin = function(){
		return this.getTimeStart().getTime()/1000;
	};
	this.getTimeEnd = function() {
		var date = new Date(( this.startTime + this.duration ) * 1000);
		return parseInt( date.getTime()/1000 );
	};
	this.getTimeEndString = function() {
		var date = new Date(( this.startTime + this.duration ) * 1000);
		var h = date.getHours();
		var m = date.getMinutes();
		if (m < 10){
			m="0"+m;
		}
		return h+":"+m;
	};
	this.getDuration = function() {
		var date = new Date( this.duration * 1000);
		return date;
	};
	this.getTimeRemainingString = function() {
		
		if( this.currentTime <= this.startTime ){
			return Math.ceil(this.getDuration() / 60000);
		} else {
			if( this.getTimeEnd() > 0){
				var remaining = Math.ceil( ( this.getTimeEnd() - this.currentTime ) / 60);
				return remaining;
			} else {
				return this.getTimeEnd();
			}
		}
	};
	
	this.getProgress = function(){
		var progress = 100 - ( ( this.getTimeRemainingString() / ( this.getDuration() / 60000 ) ) * 100 );
		progress = Math.ceil(progress);
		if(isNaN(progress))
			progress = 0;
		return progress;
	};
	
	this.getTitle = function() {
		return this.title;
	};
	this.getDescription = function() {
		return this.description;
	};
	this.getDescriptionExtended = function() {
		return this.descriptionE;
	};
	this.getServiceReference = function() {
		return encodeURIComponent(this.serviceRef);
	};
	this.getServiceName = function() {
		return this.serviceName;
	};
	
	this.json = {
			'date': this.getTimeDay(),
			'eventid': this.getEventId(),
			'servicereference': this.getServiceReference(),
			'servicename': quotes2html(this.getServiceName()),
			'title': quotes2html(this.getTitle()),
			'shorttitle': quotes2html(this.getTitle().substring(0, 40) ) + '...',
			'titleESC': escape(this.getTitle()),
			'starttime': this.getTimeStartString(), 
			'duration': Math.ceil(this.getDuration()/60000), 
			'description': quotes2html(this.getDescription()),
			'endtime': this.getTimeEndString(), 
			'remaining': this.getTimeRemainingString(),
			'progress' : this.getProgress(),
			'extdescription': quotes2html(this.getDescriptionExtended()),
			'number': String(this.number),
			'start': this.getTimeBegin(),
			'end': this.getTimeEnd()
			};
	
	this.toJSON = function() {
		return this.json;
	};
	
}
//END class EPGEvent
function EPGList(xml){
	// parsing values from xml-element
	try{
		this.xmlitems = xml.getElementsByTagName("e2eventlist").item(0).getElementsByTagName("e2event");
	} catch (e) {
		core.notify("Error Parsing EPG: " + e, false);
	}
	
	this.getArray = function(sortbytime){
		var list = [];
		var len = this.xmlitems.length;
		
		if (sortbytime === true){
			debug("[EPGList].getArray :: Sort by time!");
			var sortList = [];
			
			for(var i=0; i<len; i++){
				var event = new EPGEvent(this.xmlitems.item(i), i).toJSON();
				sortList.push( [event.starttime, event] );
			}
			sortList.sort(this.sortFunction);
			
			list = [];
			len = sortList.length;
			for(i=0; i<len; i++){
				list.push(sortList[i][1]);
			}
			
			return list;
			
		}else{
			for (i=0; i<len; i++){
				xv = new EPGEvent(this.xmlitems.item(i)).toJSON();
				list.push(xv);			
			}
			return list;
		}
	};
	
	this.sortFunction = function(a, b){
	  return a[0] - b[0];
	};
}

function EPGListNowNext(xml){
	// parsing values from xml-element
	try{
		this.xmlitems = xml.getElementsByTagName("e2eventlist").item(0).getElementsByTagName("e2event");
	} catch (e) {
		core.notify("Error Parsing EPG: " + e, false);
	}
	
	this.getArray = function(){
		list = [];
		var len = this.xmlitems.length;
		
		var idx = 0;
		var cssclass = 'even';
		
		for (var i=0; i < len; i += 2){
			cssclass = cssclass == 'even' ? 'odd' : 'even';
			now = new EPGEvent(this.xmlitems.item(i)).toJSON();
			next = new EPGEvent(this.xmlitems.item(i+1)).toJSON();
			list[idx] = {"now" : now, "next" : next, "cssclass": cssclass};
			idx++;
		}
		return list;
	};
}
//END class EPGList

// START class Service
function Service(xml, cssclass){	
	this.servicereference = getNodeContent(xml, 'e2servicereference', '');
	this.servicename = getNodeContent(xml, 'e2servicename');
	this.videowidth = getNodeContent(xml, 'e2videowidth');
	this.videoheight = getNodeContent(xml, 'e2videoheight');
	this.videosize = getNodeContent(xml, 'e2servicevideosize');
	this.widescreen = getNodeContent(xml, 'e2iswidescreen');
	this.apid = dec2hex( getNodeContent(xml, 'e2apid'),4 );
	this.vpid = dec2hex( getNodeContent(xml, 'e2vpid'),4 );
	this.pcrpid = dec2hex( getNodeContent(xml, 'e2pcrpid'),4 );
	this.pmtpid = dec2hex( getNodeContent(xml, 'e2pmtpid'),4 );
	this.txtpid = dec2hex( getNodeContent(xml, 'e2txtpid'),4 );
	this.tsid = dec2hex( getNodeContent(xml, 'e2tsid'),4 );
	this.onid = dec2hex( getNodeContent(xml, 'e2onid'),4 );
	this.sid = dec2hex( getNodeContent(xml, 'e2sid'),4 );
	
	this.getServiceReference = function(){
		return encodeURIComponent(this.servicereference);
	};
	
	this.getClearServiceReference = function(){
		return this.servicereference;
	};
		
	this.getServiceName = function(){
		return this.servicename.replace('&quot;', '"');
	};
	
	this.setServiceReference = function(sref){
		this.servicereference = sref;
	};
		
	this.setServiceName = function(sname){
		this.servicename = sname.replace('&quot;', '"');
	};
	
	if( typeof( cssclass ) == undefined ){
		cssclass = 'odd';
	}
	
	this.json = { 	
			'servicereference' : this.getServiceReference(),
			'servicename' : this.getServiceName(),
			'videowidth' : this.videowidth,
			'videoheight' : this.videoheight,
			'videosize' : this.videosize,
			'widescreen' : this.widescreen,
			'apid' : this.apid,
			'vpid' : this.vpid,
			'pcrpid' : this.pcrpid,
			'pmtpid' : this.pmtpid,
			'txtpid' : this.txtpid,
			'tsid' : this.tsid,
			'onid' : this.onid,
			'sid' : this.sid,
			'cssclass' : cssclass
	};
	
	this.toJSON = function(){
		return this.json;
	};
}	
//END class Service

// START class ServiceList
function ServiceList(xml){
	this.xmlitems = getNamedChildren(xml, "e2servicelist", "e2service");
	this.servicelist = [];
	this.getArray = function(){
		if(this.servicelist.length === 0){
			var cssclass = 'even';
			
			var len = this.xmlitems.length;
			for (var i=0; i<len; i++){
				cssclass = cssclass == 'even' ? 'odd' : 'even';
				var service = new Service(this.xmlitems.item(i), cssclass).toJSON();
				this.servicelist.push(service);
			}
		}
		
		return this.servicelist;
	};
}
//END class ServiceList


// START class Movie
function Movie(xml, cssclass){	
	this.servicereference = getNodeContent(xml, 'e2servicereference');
	this.servicename = getNodeContent(xml, 'e2servicename');
	this.title = getNodeContent(xml, 'e2title');
	this.descriptionextended = getNodeContent(xml, 'e2descriptionextended');
	this.description = getNodeContent(xml, 'e2description');
	this.tags = getNodeContent(xml, 'e2tags', '&nbsp;');
	this.filename = getNodeContent(xml, 'e2filename');
	this.filesize = getNodeContent(xml, 'e2filesize', 0);
	this.startTime = getNodeContent(xml, 'e2time', 0);
	this.length = getNodeContent(xml, 'e2length', 0);

	this.getLength = function() {
		return this.length;
	};
	
	this.getTimeStart = function() {
		var date = new Date(parseInt(this.startTime, 10)*1000);
		return date;
	};
	
	this.getTimeStartString = function() {
		var h = this.getTimeStart().getHours();
		var m = this.getTimeStart().getMinutes();
		if (m < 10){
			m="0"+m;
		}
		return h+":"+m;
	};
	
	this.getTimeDay = function() {
		var Wochentag = ["So", "Mo", "Di", "Mi", "Do", "Fr", "Sa"];
		var wday = Wochentag[this.getTimeStart().getDay()];
		var day = this.getTimeStart().getDate();
		var month = this.getTimeStart().getMonth()+1;
		var year = this.getTimeStart().getFullYear();
		
		return wday+".&nbsp;"+day+"."+month+"."+year;
	};

	this.getServiceReference = function(){
		return encodeURIComponent(this.servicereference);
	};
	this.getServiceName = function(){
		return this.servicename.replace('&quot;', '"');
	};
	
	this.getTitle = function(){
		return this.title;
	};
	
	this.getDescription = function(){
		return this.description;
	};
	
	this.getDescriptionExtended = function(){
		return this.descriptionextended;
	};
	
	this.getTags = function(){		
		return this.tags.split(" ");
	};
	
	this.getFilename = function(){		
		return encodeURIComponent(this.filename);		
	};
	
	this.getFilesizeMB = function(){		
		return Math.round((parseInt(this.filesize, 10)/1024)/1024)+"MB";
	};
	
	if( typeof( cssclass) == 'undefined'){
		cssclass = 'odd';
	}
	
	this.json = {
			'servicereference': this.getServiceReference(),
			'servicename': this.getServiceName(),
			'title': this.getTitle(),
			'escapedTitle': escape(this.getTitle()),
			'description': this.getDescription(), 
			'descriptionextended': this.getDescriptionExtended(),
			'filename': String(this.getFilename()),
			'filesize': this.getFilesizeMB(),
			'tags': this.getTags().join(', ') ,
			'length': this.getLength() ,
			'time': this.getTimeDay()+"&nbsp;"+ this.getTimeStartString(),
			'cssclass' : cssclass
	};
	
	this.toJSON = function(){
		return this.json;
	};
}	
//END class Movie


// START class MovieList
function MovieList(xml){
	this.xmlitems = getNamedChildren(xml, "e2movielist", "e2movie");
	this.movielist = [];
	
	this.getArray = function(){
		if(this.movielist.length === 0){
			var cssclass = "even";
			var len = this.xmlitems.length;
			for(var i=0; i<len; i++){
				cssclass = cssclass == 'even' ? 'odd' : 'even';
				
				var movie = new Movie(this.xmlitems.item(i), cssclass).toJSON();
				this.movielist.push(movie);			
			}
		}
		
		return this.movielist;
	};
}
//END class MovieList



// START class Timer
function Timer(xml, cssclass){	
	this.servicereference = getNodeContent(xml, 'e2servicereference');
	this.servicename = getNodeContent(xml, 'e2servicename');
	this.eventid = getNodeContent(xml, 'e2eit');
	this.name = getNodeContent(xml, 'e2name');
	this.description = getNodeContent(xml, 'e2description', '');
	this.descriptionextended = getNodeContent(xml, 'e2descriptionextended', '');
	this.disabled = getNodeContent(xml, 'e2disabled', '0');
	this.timebegin = getNodeContent(xml, 'e2timebegin');
	this.timeend = getNodeContent(xml, 'e2timeend');
	this.duration = getNodeContent(xml, 'e2duration', '0');
	this.startprepare = getNodeContent(xml, 'e2startprepare');
	this.justplay = getNodeContent(xml, 'e2justplay', '');
	this.afterevent = getNodeContent(xml, 'e2afterevent', '0');
	this.dirname = getNodeContent(xml, 'e2dirname', '/hdd/movie/');
	this.tags = getNodeContent(xml, 'e2tags', '');
	this.logentries = getNodeContent(xml, 'e2logentries');
	this.tfilename = getNodeContent(xml, 'e2filename');
	this.backoff = getNodeContent(xml, 'e2backoff');
	this.nextactivation = getNodeContent(xml, 'e2nextactivation');
	this.firsttryprepare = getNodeContent(xml, 'e2firsttryprepare');
	this.state = getNodeContent(xml, 'e2state');
	this.repeated = getNodeContent(xml, 'e2repeated', '0');
	this.dontsave = getNodeContent(xml, 'e2dontsave');
	this.cancled = getNodeContent(xml, 'e2cancled');
	this.color = getNodeContent(xml, 'e2color');
	this.toggledisabled = getNodeContent(xml, 'e2toggledisabled');
	this.toggledisabledimg = getNodeContent(xml, 'e2toggledisabledimg');

	this.getColor = function(){
		return this.color;
	};
	
	this.getToggleDisabled = function(){
		return this.toggledisabled;
	};
	
	this.getToggleDisabledIMG = function(){
		return this.toggledisabledimg;
	};
	
	this.getToggleDisabledText = function(){
		var retVal = this.toggledisabled == "0" ? "Enable" : "Disable";
		return retVal;
	};
	
	this.getServiceReference = function(){
		return encodeURIComponent(this.servicereference);
	};
	
	this.getServiceName = function(){
		return this.servicename.replace('&quot;', '"');
	};
	
	this.getEventID = function(){
		return this.eventid;
	};
	
	this.getName = function(){
		return this.name;
	};
	
	this.getDescription = function(){
		return this.description;
	};
	
	this.getDescriptionExtended = function(){
		return this.descriptionextended;
	};
	
	this.getDisabled = function(){
		return this.disabled;
	};
	
	this.getTimeBegin = function(){
		return this.timebegin;
	};
	
	this.getTimeEnd = function(){
		return this.timeend;
	};
	
	this.getDuration = function(){
		return parseInt(this.duration, 10);
	};
	
	this.getStartPrepare = function(){
		return this.startprepare;
	};
	
	this.getJustplay = function(){
		return this.justplay;
	};
	
	this.getAfterevent = function(){
		return this.afterevent;
	};
	
	this.getDirname = function(){
		return this.dirname;
	};

	this.getTags = function(){
		return this.tags;
	};

	this.getLogentries = function(){
		return this.logentries;
	};
	
	this.getFilename = function(){
		return this.tfilename;
	};
	
	this.getBackoff = function(){
		return this.backoff;
	};
	
	this.getNextActivation = function(){
		return this.nextactivation;
	};
	
	this.getFirsttryprepare = function(){
		return this.firsttryprepare;
	};
	
	this.getState = function(){
		return this.state;
	};
	
	this.getRepeated = function(){
		return this.repeated;
	};
	
	this.getDontSave = function(){
		return this.dontsave;
	};
	
	this.isCancled = function(){
		return this.cancled;
	};
	
	if( typeof( cssclass ) == undefined ){
		cssclass = 'odd';
	}
	
	this.beginDate = new Date(Number(this.getTimeBegin()) * 1000);
	this.endDate = new Date(Number(this.getTimeEnd()) * 1000);
	
	this.aftereventReadable = [ 'Nothing', 'Standby',
	                            'Deepstandby/Shutdown', 'Auto' ];
	
	this.justplayReadable = ['Record', 'Zap', 'Download Epg'];
	
	this.json = {
			'servicereference' : this.getServiceReference(),
			'servicename' : quotes2html(this.getServiceName()),
			'name' : quotes2html(this.getName()),
			'description' : quotes2html(this.getDescription()),
			'descriptionextended' : quotes2html(this.getDescriptionExtended()),
			'begin' : this.getTimeBegin(),
			'beginDate' : dateToString(this.beginDate),
			'end' : this.getTimeEnd(),
			'endDate' : dateToString(this.endDate),
			'state' : this.getState(),
			'duration' : Math.ceil((this.getDuration() / 60)),
			'repeated' : this.getRepeated(),
			'repeatedReadable' : repeatedReadable(this.getRepeated()),
			'justplay' : this.getJustplay(),
			'justplayReadable' : this.justplayReadable[Number(this.getJustplay())],
			'afterevent' : this.getAfterevent(),
			'aftereventReadable' : this.aftereventReadable[Number(this.getAfterevent())],
			'dirname' : this.getDirname(),
			'tags' : this.getTags(),
			'disabled' : this.getDisabled(),
			'onOff' : this.getToggleDisabledIMG(),
			'enDis' : this.getToggleDisabledText(),
			'cssclass' : cssclass
	};
	
	this.toJSON = function(){
		return this.json;
	};
}


// START class TimerList
function TimerList(xml){
	this.xmlitems = getNamedChildren(xml, "e2timerlist", "e2timer");
	this.timerlist = [];
	
	this.getArray = function(){
		if(this.timerlist.length === 0){
			var cssclass = 'even';
			var len = this.xmlitems.length;
			for(var i=0; i<len; i++){
				cssclass = cssclass == 'even' ? 'odd' : 'even';
				var timer = new Timer(this.xmlitems.item(i), cssclass).toJSON();
				this.timerlist.push(timer);			
			}
		}
		
		return this.timerlist;
	};
}
//END class TimerList
function DeviceInfo(xml){
	xml = xml.getElementsByTagName("e2deviceinfo").item(0);
	
	this.info = {};
	
	this.nims = [];
	this.hdds = [];
	this.nics = [];
	
	this.fpversion = "V"+xml.getElementsByTagName('e2fpversion').item(0).firstChild.data;
	
	var nimnodes = xml.getElementsByTagName('e2frontends').item(0).getElementsByTagName('e2frontend');			
	for(var i = 0; i < nimnodes.length; i++){					
		try {
			var name = nimnodes.item(i).getElementsByTagName('e2name').item(0).firstChild.data;
			var model = nimnodes.item(i).getElementsByTagName('e2model').item(0).firstChild.data;
			this.nims[i] = { 
					'name' : name, 
					'model' : model
			};					
		} catch (e) {
			core.notify("Error parsing frontend data: " + e);
		}
	}
	
	
	var hddnodes = xml.getElementsByTagName('e2hdd');
	for( var i = 0; i < hddnodes.length; i++){
		try{			
			var hdd = hddnodes.item(i);
	
			var model 	= hdd.getElementsByTagName('e2model').item(0).firstChild.data;
			var capacity = hdd.getElementsByTagName('e2capacity').item(0).firstChild.data;
			var free		= hdd.getElementsByTagName('e2free').item(0).firstChild.data;
	
			this.hdds[i] = {	
					'model'		: model,
					'capacity' 	: capacity,
					'free'		: free
			};
		} catch(e){
			core.notify("Error parsing HDD data: " + e, false);
		}
	}
	
	
	var nicnodes = xml.getElementsByTagName('e2interface');
	for( var i = 0; i < nicnodes.length; i++){
		try {
			var nic = nicnodes.item(i);
			var name = nic.getElementsByTagName('e2name').item(0).firstChild.data;
			var mac = nic.getElementsByTagName('e2mac').item(0).firstChild.data;
			var dhcp = nic.getElementsByTagName('e2dhcp').item(0).firstChild.data;
			var ip = nic.getElementsByTagName('e2ip').item(0).firstChild.data;
			var gateway = nic.getElementsByTagName('e2gateway').item(0).firstChild.data;
			var netmask = nic.getElementsByTagName('e2netmask').item(0).firstChild.data;
	
			this.nics[i] = {
					'name' : name,
					'mac' : mac,
					'dhcp' : dhcp,
					'ip' : ip,
					'gateway' : gateway,
					'netmask' : netmask
			};
		} catch (e) {
			core.notify("Error parsing NIC data: " + e, false);
		}
	}
	
	try{
		this.info = {
				'devicename' : xml.getElementsByTagName('e2devicename').item(0).firstChild.data,
				'enigmaVersion': xml.getElementsByTagName('e2enigmaversion').item(0).firstChild.data,
				'imageVersion': xml.getElementsByTagName('e2imageversion').item(0).firstChild.data,
				'fpVersion': this.fpversion,
				'webifversion': xml.getElementsByTagName('e2webifversion').item(0).firstChild.data
		};
	} catch (e) {
		core.notify("Error parsing deviceinfo data: " + e, false);		
	}
	
	this.json = {
			info : this.info,
			hdds : this.hdds,		
			nics : this.nics,
			nims : this.nims
	};
	
	this.toJSON = function(){
		return this.json;
	};
	
}

function SimpleXMLResult(xml){		
	try{
		this.xmlitems = xml;
	} catch (e) {
		core.notify("Error parsing e2simplexmlresult: " + e, false);
	}

	this.state = getNodeContent(this.xmlitems, 'e2state', false);
	this.statetext = getNodeContent(this.xmlitems, 'e2statetext', false);
	
	if(!this.state && !this.statetext){
		this.state = getNodeContent(this.xmlitems, 'e2result', 'False');
		this.statetext = getNodeContent(this.xmlitems, 'e2resulttext', 'Error Parsing XML');
	}
	
	this.getState = function(){
		if(this.state == 'True'){
			return true;
		}else{
			return false;
		}
	};
	
	this.getStateText = function(){
			return this.statetext;
	};
}
// END SimpleXMLResult

// START SimpleXMLList
function SimpleXMLList(xml, tagname){
	// parsing values from xml-element
	try{
		this.xmlitems = xml.getElementsByTagName(tagname);
	} catch (e) {
		core.notify("Error parsing SimpleXMLList: " + e, false);	
	}
	
	this.xmllist = [];
	
	this.getList = function(){
		if(this.xmllist.length === 0){
			var len = this.xmlitems.length;
			for(var i=0; i<len; i++){
				this.xmllist.push(this.xmlitems.item(i).firstChild.data);
			}
		}
		
		return this.xmllist;
	};
}
// END SimpleXMLList


// START class Setting
function Setting(xml){	
	this.settingvalue = getNodeContent(xml, 'e2settingvalue');
	this.settingname = getNodeContent(xml, 'e2settingname');
	
	this.getSettingValue = function(){
		return this.settingvalue;
	};
		
	this.getSettingName = function(){
		return this.settingname;
	};
	
}


// START class Settings
function Settings(xml){
	// parsing values from xml-element
	try{
		this.xmlitems = xml.getElementsByTagName("e2settings").item(0).getElementsByTagName("e2setting");
		debug("[Settings] Number of items: " + this.xmlitems);
	} catch (e) {
		core.notify("Error parsing Settings: " + e, false);	
	}	
	
	this.settings = [];
	
	this.getArray = function(){
		if(this.settings.length === 0){
			var len = this.xmlitems.length;
			for (var i=0; i < len; i++){
				var setting = new Setting(this.xmlitems.item(i));
				this.settings.push(setting);			
			}
		}
		
		return this.settings;		
	};
}
//END class Settings

//START class FileList
function FileList(xml){
	// parsing values from xml-element
	try{
		this.xmlitems = xml.getElementsByTagName("e2filelist").item(0).getElementsByTagName("e2file");
	} catch (e) {
		core.notify("Error parsing FileList: " + e, false);
	}
	this.filelist = [];

	this.getArray = function(){
		if(this.filelist.length === 0){
			var len = this.xmlitems.length;
			for(var i = 0; i < len; i++){
				var file = new File(this.xmlitems.item(i));
				this.filelist.push(file);
			}
		}

		return this.filelist;
	};
}
//END class FileList

//START class File
function File(xml){	
	// parsing values from xml-element
	this.servicereference = getNodeContent(xml, 'e2servicereference', 'Filesystems');
	this.isdirectory = getNodeContent(xml, 'e2isdirectory');
	this.root = getNodeContent(xml, 'e2root', 'Filesystems');

	this.getServiceReference = function(){
		return this.servicereference;
	};
	
	this.getNameOnly = function(){
		if(this.root == '/' || this.isdirectory == "True") {
			return this.servicereference.replace(this.root, '');
		} else {
			return this.servicereference.replace('4097:0:0:0:0:0:0:0:0:0:', '').replace(this.root, '');
		}
	};
	
	this.getIsDirectory = function(){
		return this.isdirectory;
	};
	
	this.getRoot = function(){
		return this.root;
	};
}	
//END class File

//class Volume
var Vol = Class.create({
	initialize: function(xml){
		this.result = getNodeContent(xml, 'e2result', 'False');
		this.resultText = getNodeContent(xml, 'e2resulttext', 'Parser Error!');
		this.volume = getNodeContent(xml, 'e2current', '100');		
		var mute = getNodeContent(xml, 'e2ismuted', 'False');
		this.isMuted = mute == "True" ? true : false;
		this.states = [];
		
		for(var i = 1; i <= 10; i++){
			var enabled = false;
			if(this.volume >= i * 10){
				enabled = true;
			}		
			this.states[i-1] = {'enabled' : enabled, 'percent' : i*10};
		}
		
		this.json = {
				result : this.result,
				resultText : this.resultText,
				volume : this.volume,
				isMuted : this.isMuted,
				states : this.states
		};
	},

	toJSON: function(){
		return this.json;
	}
});

var Powerstate = Class.create({
	initialize: function(xml){
		this.instandby = (getNodeContent(xml, "e2instandby").strip() == "true");
	},
	
	isStandby: function(){
		return this.instandby;
	}
});

var Signal = Class.create({
	initialize: function(xml){
		this.snrdb = getNodeContent(xml, 'e2snrdb');
		this.snr = getNodeContent(xml, 'e2snr');
		this.ber = getNodeContent(xml, 'e2ber');
		this.acg = getNodeContent(xml, 'e2acg');
		
		this.json = {
			'snrdb' : this.snrdb,
			'snr' : this.snr,
			'ber' : this.ber,
			'acg' : this.acg
		};
	},
	
	toJSON: function(){
		return this.json;
	}

});
//END class Volume