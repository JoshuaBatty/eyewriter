#include "pocoHttpPostForm.h"
#include <sstream>
#include <iomanip>
#include <cctype>

namespace {

// Minimal URL encoder for x-www-form-urlencoded
static std::string urlEncode(const std::string& s) {
	std::ostringstream oss;
	oss.fill('0');
	oss << std::hex << std::uppercase;

	for (unsigned char c : s) {
		if ((c >= 'A' && c <= 'Z') ||
			(c >= 'a' && c <= 'z') ||
			(c >= '0' && c <= '9') ||
			c == '-' || c == '_' || c == '.' || c == '~') {
			oss << c;
		} else if (c == ' ') {
			oss << '+';
		} else {
			oss << '%' << std::setw(2) << int(c);
		}
	}
	return oss.str();
}

static std::string buildFormBody(const std::vector<std::string>& names,
								 const std::vector<std::string>& values) {
	std::ostringstream body;
	for (size_t i = 0; i < names.size(); ++i) {
		if (i) body << '&';
		body << urlEncode(names[i]) << '=' << urlEncode(values[i]);
	}
	return body.str();
}

} // namespace

pocoHttpPostForm::pocoHttpPostForm() {
	timeoutSeconds = 40; // keep original default
}

pocoHttpPostForm::~pocoHttpPostForm() {}

void pocoHttpPostForm::setUrl(std::string surl) {
	url = std::move(surl);
}

void pocoHttpPostForm::addForm(std::string name, std::string value) {
	formNames.push_back(std::move(name));
	formValues.push_back(std::move(value));
}

void pocoHttpPostForm::clearFormData() {
	lock();
	formNames.clear();
	formValues.clear();
	unlock();
}

void pocoHttpPostForm::submit() {
	bSubmitting = true;
	NonThreadedFunction();
	bSubmitting = false;
}

void pocoHttpPostForm::threadedFunction() {
	// Kept empty to match legacy behavior
}

bool pocoHttpPostForm::wasLastSubmitted() {
	return bLastFormSubmitted;
}

void pocoHttpPostForm::NonThreadedFunction() {
	bLastFormSubmitted = false;

	if (url.empty()) {
		ofLogError("pocoHttpPostForm") << "URL is empty; aborting.";
		return;
	}

	// Build x-www-form-urlencoded body from form fields
	const std::string body = buildFormBody(formNames, formValues);

	// Prepare request using oF’s HTTP structures
	ofHttpRequest req;
	req.url            = url;
	req.method         = ofHttpRequest::POST;                 // POST
	req.contentType    = "application/x-www-form-urlencoded";
	req.body           = body;
	req.timeoutSeconds = static_cast<size_t>(std::max(0, timeoutSeconds));

	// Optional: add headers (User-Agent helps with some endpoints)
	req.headers["User-Agent"] = "openFrameworks/URLFileLoader";

	// Synchronous request; returns ofHttpResponse
	// Note: we instantiate a loader locally; you can also use the global helpers.
	ofURLFileLoader loader;
	ofHttpResponse resp = loader.handleRequest(req);

	// Consider 2xx as success
	bLastFormSubmitted = (resp.status >= 200 && resp.status < 300);

	if (!bLastFormSubmitted) {
		ofLogWarning("pocoHttpPostForm")
			<< "HTTP POST to " << url << " failed, status=" << resp.status
			<< " error=" << resp.error;
	}
}


//#include "pocoHttpPostForm.h"
//
//
//pocoHttpPostForm::pocoHttpPostForm()
//{
//	bLastFormSubmitted	= false;
//	bSubmitting			= false;
//	timeoutSeconds		= 40;		// default timeout
//
//}
//
//pocoHttpPostForm::~pocoHttpPostForm()
//{
//    //dtor
//}
//
//void pocoHttpPostForm::setUrl( string surl )
//{
//	url.clear();
//	url = surl;
//}
//
//void pocoHttpPostForm::submit()
//{
//
//	bSubmitting = true;
//		NonThreadedFunction();
//	bSubmitting = false;
//
//}
//
//void pocoHttpPostForm::clearFormData()
//{
//	lock();
//		formNames.clear();
//		formValues.clear();
//	unlock();
//}
//
//void pocoHttpPostForm::addForm( string name, string value )
//{
//	formNames.push_back(name);
//	formValues.push_back(value);
//
//}
//
//void pocoHttpPostForm::threadedFunction(){
//
//}
//
//void pocoHttpPostForm::NonThreadedFunction()
//{
//			try
//			{
//				cout << "trying to submit" << endl;
//
//				bLastFormSubmitted = false;
//
//				URI uri( url );
//				std::string path(uri.getPathAndQuery());
//				if (path.empty()) path = "/";
//
//				HTTPClientSession session(uri.getHost(), uri.getPort());
//				HTTPRequest req(HTTPRequest::HTTP_POST, path, HTTPMessage::HTTP_1_1);
//
//				session.setTimeout(Poco::Timespan(timeoutSeconds,0));
//
//				// create the form data to send
//				HTMLForm pocoForm(HTMLForm::ENCODING_URL);
//
//				// form values
//				for(unsigned int i=0; i< formNames.size(); i++){
//					const std::string name = formNames[i].c_str();
//					const std::string val  = formValues[i].c_str();
//					pocoForm.set(name, val);
//				}
//
//				pocoForm.prepareSubmit(req);
//
//				printf("pocoForm writing\n");
//
//				try{
//				    pocoForm.write(session.sendRequest(req));
//
//                    //HTTPResponse res;
//                    //istream& rs = session.receiveResponse(res);
//                    //cout << "rs " << res.getReason() << endl;
//
//                    //if( res.getStatus() == 200 )
//
//				}catch (Exception& exc){
//                    printf("pocoForm write error \n");
//				}
//
//                bLastFormSubmitted = true;
//				bSubmitting = false;
//
//			}catch (Exception& exc){
//				cout << "Exception thrown!" << endl;
//				// ofxHttpEvents.notifyNewError("time out ");
//			   // if(verbose) std::cerr << exc.displayText() << std::endl;
//			}
//
//}
//
//
//bool pocoHttpPostForm::wasLastSubmitted(){
//	return true;
//}
//
