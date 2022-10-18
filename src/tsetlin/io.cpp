#include "io.h"


bool
DNA2SIG (std::string raw_inputpath, std::string raw_outputpath,
         int buffer_size, bool cleave_zero)
{   // (A)denine represented by    -1
    // (T)hymine represented by     1
    // (C)ytosine represented by   -2
    // (G)uanine represented by     2
    std::ifstream input (raw_inputpath);
    std::ofstream output (raw_outputpath);
    int buffer_idx = 0;
    if (buffer_size == 0)
    {
        std::ifstream fastseek (raw_inputpath, std::ios::binary | std::ios::ate);
        buffer_size = fastseek.tellg ();
        fastseek.close ();
        std::cout << "Chosed to load whole file, costs " << buffer_size
                << " bytes of memory.\n";
    }
    float *buffer = (float *)malloc (buffer_size * sizeof (float));
    if (buffer != NULL)
    {
        std::cout << "Output buffer successfully allocated in size of :"
                    << buffer_size * sizeof (float) / (double)(1024 * 1024) << "MB"
                    << "\n";
    }
    else
    {
        std::cerr
            << "Buffer allocation failed, please chose another buffer size!"
            << "\n";
    }

  char c;
  while (input.get (c))
    {
      // Load the buffer
        switch (c)
        {
            case 'A':
            buffered_output<int, float> (-1, buffer_idx, buffer_size, buffer,
                                       output);
            break;
            case 'a':
            buffered_output<int, float> (-1, buffer_idx, buffer_size, buffer,
                                       output);
            break;
            case 'T':
            buffered_output<int, float> (1, buffer_idx, buffer_size, buffer,
                                       output);
            break;
            case 't':
            buffered_output<int, float> (1, buffer_idx, buffer_size, buffer,
                                       output);
            break;
            case 'C':
            buffered_output<int, float> (-2, buffer_idx, buffer_size, buffer,
                                       output);
            break;
            case 'c':
            buffered_output<int, float> (-2, buffer_idx, buffer_size, buffer,
                                       output);
            break;
            case 'G':
            buffered_output<int, float> (2, buffer_idx, buffer_size, buffer,
                                       output);
            break;
            case 'g':
            buffered_output<int, float> (2, buffer_idx, buffer_size, buffer,
                                       output);
            break;
            default: // Intend to cut the hard mask because it provide no
                 // information.
            if (!cleave_zero)
            buffered_output<int, float> (0, buffer_idx, buffer_size, buffer,
                                         output);
          break;
        }
    }
    if (buffer_idx != 0) // Clean the remaining buffer if not empty.
    {
        output.write ((char *)buffer, (buffer_idx) * sizeof (float));
    }
  free (buffer);
  return COMPLETED;
}

bool
write_csv_row (std::vector<float> data, std::ofstream *output)
{
    int i;
    for (i = 0; i < data.size () - 1; i++)
    {
        (*output) << data[i] << ",";
    }
    (*output) << data[i] << "\n";
    return COMPLETED;
}

std::vector<int>
siRNA2SIG (std::string raw_string)
{
  std::vector<int> result;
  size_t current_idx = 0;
  for (char c : raw_string)
    {
      switch (c)
        {
        case 'A':
          result.insert (result.end (), { 0, 0, 0, 1 });
          break;
        case 'U':
          result.insert (result.end (), { 0, 0, 1, 0 });
          break;
        case 'C':
          result.insert (result.end (), { 0, 1, 0, 0 });
          break;
        case 'G':
          result.insert (result.end (), { 1, 0, 0, 0 });
          break;
        case 'a':
          result.insert (result.end (), { 0, 0, 0, 1 });
          break;
        case 't':
          result.insert (result.end (), { 0, 0, 1, 0 });
          break;
        case 'c':
          result.insert (result.end (), { 0, 1, 0, 0 });
          break;
        case 'g':
          result.insert (result.end (), { 1, 0, 0, 0 });
          break;
        default: // Not expected other characters.
          break;
        }
    }
  return result;
}
void
parse_huesken_seqs (std::string path, std::vector<std::vector<int> > &result)
{
    std::ifstream seqfile (path);
    std::string thisline;
    int row = 0;
    while (std::getline (seqfile, thisline))
    {
        result[row++] = siRNA2SIG (thisline);
    }
}

void
parse_huesken_scores (std::string path, std::vector<std::vector<int> > &result)
{
    std::ifstream score_file (path);
    std::string score_string;
    float this_score;
    int idx = 0;
    while (std::getline (score_file, score_string))
    {
        this_score = ::atof (score_string.c_str ());
        if (this_score < 0.3)
            result[idx++] = { 0, 0, 0, 1 };
        if (this_score >= 0.3 && this_score < 0.5)
            result[idx++] = { 0, 0, 1, 0 };
        if (this_score >= 0.5 && this_score < 0.7)
            result[idx++] = { 0, 1, 0, 0 };
        if (this_score >= 0.7)
            result[idx++] = { 1, 0, 0, 0 };
    }
}

void
modelOutput (TsetlinMachine tm,
             std::vector<TsetlinMachine::Clause> bestPositiveClauses,
             std::vector<TsetlinMachine::Clause> bestNegativeClauses,
             float precision,
             std::string outputpath)
{
    int automataPerClause = bestPositiveClauses[0]._automataStates.size ();
    int *positive = (int *)malloc (bestPositiveClauses.size ()
                                 * automataPerClause * sizeof (int));
    int *negative = (int *)malloc (bestPositiveClauses.size ()
                                 * automataPerClause * sizeof (int));
    std::cout << bestNegativeClauses.size () << " rows \t" << automataPerClause
            << " columns" << std::endl;
    int clauseIdx = 0;
    for (auto clause : bestPositiveClauses)
    {
        for (int automataIdx = 0; automataIdx < automataPerClause; automataIdx++)
        {
            if (clause._inclusions.find (automataIdx)
                != clause._inclusions.end ())
            {
                positive[clauseIdx * automataPerClause + automataIdx]
                        = bestPositiveClauses[clauseIdx]
                            ._automataStates[automataIdx];
            }
        }
        clauseIdx++;
    }
    std::cout << "Negative Clauses:" << std::endl;
    clauseIdx = 0;
    for (auto clause : bestNegativeClauses)
    {
        for (int automataIdx = 0; automataIdx < automataPerClause; automataIdx++)
        {
            if (clause._inclusions.find (automataIdx)
                != clause._inclusions.end ())
            {
                negative[clauseIdx * automataPerClause + automataIdx]
                        = bestNegativeClauses[clauseIdx]
                            ._automataStates[automataIdx];
            }
        }
        clauseIdx++;
    }
    write_csv<int> (positive, bestPositiveClauses.size (), automataPerClause,
                    outputpath + "./positive_" + std::to_string(precision));
    write_csv<int> (negative, bestNegativeClauses.size (), automataPerClause,
                    outputpath + "/negative_" + std::to_string(precision));
}