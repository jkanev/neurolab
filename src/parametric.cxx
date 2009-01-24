


#include "../h/parametric.hxx"


uint Parametric::parametricOutputIndent = 0;
bool Parametric::parametricOutputLineEnd = true;

//__________________________________________________________________________
// Create object of type and name.

Parametric::Parametric(const string& name, const string& type)
{
	parametricType = type;
	parametricName = name;
};
	

//__________________________________________________________________________
// Add parameter.
// Adds another string definition to the class. If the parameter already exists, it is not added.

void Parametric::addParameter( const string& name ) {
	bool found = false;
	for (uint i=0; i<parametricStrings.size(); ++i) {
		if (parametricStrings[i] == name) {
			found = true;
			break;
		}
	}
		
	if (!found)
		parametricStrings.push_back(name);
};


//__________________________________________________________________________
// String with all parameter settings.

string Parametric::getAllParameters() const
{
	++parametricOutputIndent;
	string params;
	string indent = "";
	for (uint i=0; i<parametricOutputIndent; ++i)
		indent += "\t";
	for (uint i=0; i<parametricStrings.size(); ++i) {
		parametricOutputLineEnd = true;
		params += indent + parametricStrings[i] + ": " + getParameter( parametricStrings[i] );
		if (parametricOutputLineEnd) {
			params += "\n";
			parametricOutputLineEnd = false;
		}
	}
	--parametricOutputIndent;
	return params;
};


//__________________________________________________________________________
// Remove a parameter.

void Parametric::rmParameter( const string& name )
{
	for (uint i=0; i<parametricStrings.size(); ++i) {
		if (parametricStrings[i] == name) {
			parametricStrings.erase( parametricStrings.begin() + i );
			break;
		}
	}
};


//__________________________________________________________________________
// Returns the configuration of the object.

string Parametric::getConfiguration() const
{
	string config;
	config += "\"" + parametricName + "\" " + parametricType + '\n' + getAllParameters();
	return config;
};


//__________________________________________________________________________
// Set parameter.

void Parametric::setParameter( const string& name, const string& value)
{
	cout << "error: parameter " << name << " not available" << endl;
};


//__________________________________________________________________________
// Get parameter.

string Parametric::getParameter( const string& name ) const
{
	cout << "error: parameter " << name << " not available" << endl;
	return "??";
}
