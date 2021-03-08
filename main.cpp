#include <iostream>
#include <regex>
#include <cassert>

using namespace std;

enum class DOW { 
    Sunday, Monday, Tuesday, Wednesday, Thursday, Friday, Saturday
};

constexpr const char* ToString(DOW d)
{
    switch(d)
    {
	default:
	case DOW::Sunday: return "Sunday";
	case DOW::Monday: return "Monday";
	case DOW::Tuesday: return "Tuesday";
	case DOW::Wednesday: return "Wednesday";
	case DOW::Thursday: return "Thursday";
	case DOW::Friday: return "Friday";
	case DOW::Saturday: return "Saturday";
    }
}

constexpr int GetDaysInMonth(int month, bool bLeap)
{
    assert(month >= 1 && month <=12);
    switch(month)
    {
	// feb has extra day 
	case 2:
	    return bLeap ? 29 : 28;
	// 30 days have september,april, june, november
	case 4:
	case 6:
	case 9:
	case 11:
	    return 30;

	// 31 all rest
	default:
	    return 31;
    }	
}

static_assert(GetDaysInMonth(1,false) == 31, "January has 31 days");
static_assert(GetDaysInMonth(1,true) == 31, "January has 31 days");
static_assert(GetDaysInMonth(2,false) == 28, "Feb has 28 days (with no leap)");
static_assert(GetDaysInMonth(2,true) == 29, "Feb has 29 days (with a leap)");

constexpr int DayAndMonthToDayOfYear(int day, int month, bool bLeap)
{
    assert(day >= 1 && day <= GetDaysInMonth(month,bLeap));

    int DaysSoFar = 0;
    for(int i = 1; i < month; ++i)
	DaysSoFar += GetDaysInMonth(i,bLeap);

    return (DaysSoFar + day);
}

static_assert(DayAndMonthToDayOfYear(1,1,false) == 1, "First day is day 1");
static_assert(DayAndMonthToDayOfYear(31,12,false) == 365, "Last day is day 365 (no leap)");
static_assert(DayAndMonthToDayOfYear(31,12,true) == 366, "Last day is day 366 (with leap)");
static_assert(DayAndMonthToDayOfYear(6,5,false) == 126, "May 6th is 126 (without leap)");
static_assert(DayAndMonthToDayOfYear(6,5,true) == 127, "May 6th is 127 (with leap)");

constexpr bool IsLeap(int year)
{
    bool bDivBy4 = year % 4 == 0;
    bool bDivBy100 = year % 100 == 0;
    bool bDivBy400 = year % 400 == 0;

    return bDivBy4 && (!bDivBy100 || bDivBy400);
}

int NumOfDaysInYear(int Year)
{
    return IsLeap(Year) ? 366 : 365;
}

struct Date
{
    int Day;
    int Month;
    int Year;
    int JulianDate;

    constexpr Date(int InDay, int InMonth, int InYear)
	: Day{InDay}, Month{InMonth}, Year{InYear}
	, JulianDate{DayAndMonthToDayOfYear(InDay,InMonth,IsLeap(InYear))}
    {}

    constexpr Date operator+(int DeltaDays) const
    {
	return Date{Day+DeltaDays,Month,Year};
    }
    constexpr Date operator-(int DeltaDays) const
    {
	return Date{Day-DeltaDays,Month,Year};
    }

    constexpr int64_t DeltaJulianDays(const Date& d) const
    {
	int JulianDiff = JulianDate - d.JulianDate;
	
	// brute force O(n)
	int DeltaYears = Year - d.Year;
	int DeltaSgn = DeltaYears > 0 ? 1 : -1;

	// tally all years in range.
	int64_t DaysTally = 0;
	for(int i = Year; i != d.Year; i-=DeltaSgn)
	    DaysTally  += NumOfDaysInYear(i);

	DaysTally*=DeltaSgn;

	return DaysTally + JulianDiff;
    }
};

static constexpr double DaysPerYear = 365.2425;
static constexpr Date KnownSunday{7,3,2021}; // Sunday 0.

constexpr int64_t absint(int64_t x) { return x < 0 ? -x : x; }

constexpr DOW CalculateDOW(Date d)
{
    int64_t DeltaDays = d.DeltaJulianDays(KnownSunday);
    int64_t AbsDelta = absint(DeltaDays); 

    int Dow = DeltaDays % 7;

    return static_cast<DOW>(Dow);
}

static_assert(KnownSunday.DeltaJulianDays(KnownSunday) == 0,"Diff of same is 0");
static_assert((KnownSunday+1).DeltaJulianDays(KnownSunday) == 1,"Diff of same is 1");
static_assert((KnownSunday+2).DeltaJulianDays(KnownSunday) == 2,"Diff of same is 2");
static_assert((KnownSunday+3).DeltaJulianDays(KnownSunday) == 3,"Diff of same is 3");
static_assert((KnownSunday+4).DeltaJulianDays(KnownSunday) == 4,"Diff of same is 4");
static_assert((KnownSunday+5).DeltaJulianDays(KnownSunday) == 5,"Diff of same is 5");
static_assert((KnownSunday+6).DeltaJulianDays(KnownSunday) == 6,"Diff of same is 6");

static_assert((KnownSunday-1).DeltaJulianDays(KnownSunday) == -1,"Diff of same is 1");
static_assert((KnownSunday-2).DeltaJulianDays(KnownSunday) == -2,"Diff of same is 2");
static_assert((KnownSunday-3).DeltaJulianDays(KnownSunday) == -3,"Diff of same is 3");
static_assert((KnownSunday-4).DeltaJulianDays(KnownSunday) == -4,"Diff of same is 4");
static_assert((KnownSunday-5).DeltaJulianDays(KnownSunday) == -5,"Diff of same is 5");
static_assert((KnownSunday-6).DeltaJulianDays(KnownSunday) == -6,"Diff of same is 6");

int main()
{
    string s;
    cout << "Enter Birthday (yyyy/mm/dd)" << endl;
    cin >> s;

    regex date_re("([12]\\d{3})-(0[1-9]|1[0-2])-(0[1-9]|[12]\\d|3[01])");
    smatch match; 
    regex_match(s,match,date_re);
    
    // valid date?
    if(match.size() == 4)
    {
	int year = stoi(match[1]);
	int month = stoi(match[2]);
	int day =  stoi(match[3]);
	cout << "Valid date" << endl;	

	Date date = {day,month,year};
	DOW d = CalculateDOW(date);
	//cout << "Day of week is: " << ToString(d) << endl;
	int64_t DeltaDays = date.DeltaJulianDays(KnownSunday);
	int DeltaMod7 = DeltaDays%7;
	if(DeltaMod7 < 0) DeltaMod7+=7;
	cout << "DeltaDays=" << DeltaDays << endl;
	cout << "DeltaDays Mod 7=" << DeltaMod7 << endl;
	cout << "Result=" << ToString(static_cast<DOW>(DeltaMod7)) << endl;
    }

    return 0;
}
