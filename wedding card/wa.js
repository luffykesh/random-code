const { BrowserWindow } = require('electron');
const{app} = require('electron');

 // var remote = require('electron').remote,
 //      arguments = remote.getGlobal('sharedObject').prop1;

let ph = process.argv[2];
console.log("number: "+ph);
if(ph == undefined)
{
	console.log("Please provide phone number");
	process.exit(1);
}
app.on('ready', function() {
		let win = new BrowserWindow({ width: 800, height: 600,show:false})
		win.on('closed', () => {
		  win = null
		})

		win.once('ready-to-show', () => {
		  // win.show()
		})


    var webContents = win.webContents;
    // webContents.on('did-start-loading', function() {
    //     console.log('did-start-loading');
    // });
    webContents.on('did-stop-loading', function() {
        console.log('did-stop-loading');
			setTimeout(()=>{win.close();},1000);
    });
    // webContents.on('did-finish-load', function() {
	   //  console.log('did-finish-load');
    // });
    // webContents.on('did-get-response-details', function(e, status, newUrl) {
    //     console.log('>', newUrl);
    // });

	// Load a remote URL
	win.loadURL('whatsapp://send?phone='+ph);
	// win.loadURL('whatsapp://send?phone='+'91'+ph);
	// win.loadURL('https://api.whatsapp.com/send?phone='+ph);
});
