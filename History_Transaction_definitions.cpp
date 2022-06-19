#include <cassert>
#include <iomanip>
#include <iostream>

#include "project4.hpp"
#include "History.hpp"
#include "Transaction.hpp"

////////////////////////////////////////////////////////////////////////////////
// Definitions for Transaction class
////////////////////////////////////////////////////////////////////////////////
//
//

// Constructor
// TASK 1
//

Transaction::Transaction (std::string ticker_symbol, 
                          unsigned int day_date, 
                          unsigned int month_date, 
                          unsigned year_date, 
                          bool buy_sell_trans, 
                          unsigned int number_shares, 
                          double trans_amount)
{ 
  //initalization values of the objects
  symbol = ticker_symbol;
  day = day_date;
  month = month_date;
  year = year_date;

  acb= 0;
  cgl = 0;
  acb_per_share = 0;
  share_balance = 0;

  p_next = nullptr;
  
  if (buy_sell_trans == true) 
    trans_type ="Buy";
  else 
    trans_type = "Sell";
  shares = number_shares;
  amount = trans_amount;
  trans_id = assigned_trans_id;
  ++assigned_trans_id;
}

// Destructor
// TASK 1
//
Transaction::~Transaction () {

}

// Overloaded < operator.
// TASK 2
//

bool Transaction::operator < (Transaction const & other) {

  if (year < other.year) {
    return true;
  }
  else if (year == other.year && month < other.month) {
    return true;
  }
  else if (year == other.year && month == other.month && day < other.day) {
    return true;
  }
  return false;
}

// GIVEN
// Member functions to get values.
//
std::string Transaction::get_symbol() const { return symbol; }
unsigned int Transaction::get_day() const { return day; }
unsigned int Transaction::get_month() const { return month; }
unsigned int Transaction::get_year() const { return year; }
unsigned int Transaction::get_shares() const { return shares; }
double Transaction::get_amount() const { return amount; }
double Transaction::get_acb() const { return acb; }
double Transaction::get_acb_per_share() const { return acb_per_share; }
unsigned int Transaction::get_share_balance() const { return share_balance; }
double Transaction::get_cgl() const { return cgl; }
bool Transaction::get_trans_type() const { return (trans_type == "Buy") ? true: false ; }
unsigned int Transaction::get_trans_id() const { return trans_id; }
Transaction *Transaction::get_next() { return p_next; }

// GIVEN
// Member functions to set values.
//
void Transaction::set_acb( double acb_value ) { acb = acb_value; }
void Transaction::set_acb_per_share( double acb_share_value ) { acb_per_share = acb_share_value; }
void Transaction::set_share_balance( unsigned int bal ) { share_balance = bal ; }
void Transaction::set_cgl( double value ) { cgl = value; }
void Transaction::set_next( Transaction *p_new_next ) { p_next = p_new_next; }

// GIVEN
// Print the transaction.
//
void Transaction::print() {
  std::cout << std::fixed << std::setprecision(2);
  std::cout << std::setw(4) << get_trans_id() << " "
    << std::setw(4) << get_symbol() << " "
    << std::setw(4) << get_day() << " "
    << std::setw(4) << get_month() << " "
    << std::setw(4) << get_year() << " ";


  if ( get_trans_type() ) {
    std::cout << "  Buy  ";
  } else { std::cout << "  Sell "; }

  std::cout << std::setw(4) << get_shares() << " "
    << std::setw(10) << get_amount() << " "
    << std::setw(10) << get_acb() << " " << std::setw(4) << get_share_balance() << " "
    << std::setw(10) << std::setprecision(3) << get_acb_per_share() << " "
    << std::setw(10) << std::setprecision(3) << get_cgl()
    << std::endl;
}


////////////////////////////////////////////////////////////////////////////////
// Definitions for the History class
////////////////////////////////////////////////////////////////////////////////
//
//


// Constructor
// TASK 3
//
History::History() {
  this->p_head = nullptr;
}


// Destructor
// TASK 3
//
History::~History() {
  // followed instructional tutorial by David Lau in lab session
  Transaction * p_tmp{p_head};

  while(p_tmp != nullptr) {
    p_tmp = p_tmp->get_next();
    delete p_head;
    p_head=p_tmp;
  }
}


// read_history(...): Read the transaction history from file.
// TASK 4
//

void History::read_history() {
  ece150::open_file();

  while(ece150::next_trans_entry() == true) {
    Transaction *T = {new Transaction (ece150::get_trans_symbol(), 
                                       ece150::get_trans_day(),
                                       ece150::get_trans_month(),
                                       ece150::get_trans_year(),
                                       ece150::get_trans_type(),
                                       ece150::get_trans_shares(),
                                       ece150::get_trans_amount())}; 
  //call insert
    this->insert(T);
  }

  ece150::close_file();
}

// insert(...): Insert transaction into linked list.
// TASK 5
//

void History::insert(Transaction *p_new_trans) {
  // followed instructional tutorial by David Lau in lab session
  if (p_head == nullptr) {
    this->p_head = p_new_trans;
  } else {
    Transaction * p_next{p_head};
    
    while(p_next->get_next() != nullptr) {
      p_next = p_next->get_next();
    }

    p_next->set_next(p_new_trans);
  }
  
}

// sort_by_date(): Sort the linked list by trade date.
// TASK 6
//

void History::sort_by_date() {
  
  Transaction * p_sorted{nullptr};

  while (p_head != nullptr) {
    Transaction * p_holder{p_head};
    p_head = p_head ->get_next();
    p_holder->set_next(nullptr);


    if (p_sorted == nullptr) {
    //insert node to the front 
      p_sorted = p_holder;
      p_holder= nullptr;
    } else {
      //insert at the very front 
      if (*p_holder < *p_sorted) {
        p_holder->set_next(p_sorted);
        p_sorted = p_holder;

      } else {
        Transaction * p_next{p_sorted};
        while ((p_next->get_next() != nullptr) && (*(p_next->get_next()) < *p_holder)) {
          p_next = p_next->get_next();
        }

        p_holder ->set_next(p_next->get_next());
        p_next->set_next(p_holder);
        p_holder = nullptr;
        p_next = nullptr;
      }
    }  
  } 
  p_head = p_sorted;
}

// update_acb_cgl(): Updates the ACB and CGL values.
// TASK 7
//

void History::update_acb_cgl() {
  // For BUYS
  // acb_for_current_transaction = acb_for_previous transaction + amount_of_current_transaction
  //share_balance_for_current_transaction = share_balance_for_previous_transaction + num_shares_bought
  //acb_per_share = acb_for_current_transaction / share_balance_for_current_transaction
  //cgl_for_current_transaction = 0.0

  //For SELLS 
  //acb_for_current_transaction = acb_for_previous_transaction - acb_per_share_for_previous_transac*num_shares_sold
  //share_balance_for_current_transaction = share_balance_for_previous_transaction - num_shares_sold
  //acb_per_share_for_current_transac = acb_per_share_for_prev_transac
  //cgl_for_current_transaction = amount_for_current_transaction - num_share_sold*acb_per_share_for_current_transac
  Transaction * p_current{p_head};
  double current_acb = 0.0;
  unsigned int current_share_balance = 0;
  double current_acb_per_share = 0.0;
  double current_cgl = 0.0;

  while (p_current!= nullptr) {

    if (p_current->get_trans_type() ==  true) {

      current_acb = current_acb + p_current->get_amount();
      current_share_balance = current_share_balance + p_current->get_shares();
      current_acb_per_share = current_acb / current_share_balance;
      current_cgl = 0.0;

      
    } else {

      current_acb = current_acb - current_acb_per_share * p_current->get_shares();
      current_share_balance = current_share_balance - p_current->get_shares();
      //current_acb_per_share = current_acb_per_share;
      current_cgl = p_current->get_amount() - p_current->get_shares() * current_acb_per_share;

    }

    p_current->set_acb(current_acb);
    p_current->set_share_balance(current_share_balance);
    p_current->set_acb_per_share(current_acb_per_share);
    p_current->set_cgl(current_cgl);

    p_current = p_current ->get_next();

  }

}


// compute_cgl(): )Compute the CGL.
// TASK 8

double History::compute_cgl(unsigned int year) {
  Transaction * p_current{p_head};
  double cgl_of_year{0.0};

  while (p_current != nullptr) {
    if (p_current->get_year() == year) {
      cgl_of_year += p_current->get_cgl();
    } 
    p_current = p_current-> get_next();

  }

  return cgl_of_year;
}

// print() Print the transaction history.
//TASK 9
//

void History::print () {
  std::cout<<"========== BEGIN TRANSACTION HISTORY =========="<<std::endl;
  Transaction *p_next = p_head;
  while(p_next!=nullptr){
    p_next->print();
    p_next=p_next->get_next();
    }
  std::cout<<"========== END TRANSACTION HISTORY =========="<<std::endl;
}


// GIVEN
// get_p_head(): Full access to the linked list.
//
Transaction *History::get_p_head() { return p_head; }
