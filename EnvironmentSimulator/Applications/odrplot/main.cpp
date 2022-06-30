/*
 * esmini - Environment Simulator Minimalistic
 * https://github.com/esmini/esmini
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (c) partners of Simulation Scenarios
 * https://sites.google.com/view/simulationscenarios
 */

 /*
  * This application produces a raw data file to be used in Python for plotting the road network as simple graphs.
  *
  * This module reads an OpenDRIVE file and subsamples all roads within the road network.
  * The resulting points are saved into a data file.
  * Use separate Python program xodr.py to plot the result.
  */

#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include "RoadManager.hpp"
#include "CommonMini.hpp"

using namespace std;
using namespace roadmanager;

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		printf("Usage: AppendLaneId input_file openDriveFile.xodr\n");
		return -1;
	}
	std::string output_file_name = "out.csv";
	std::fstream fin, fout;

	string traj_file_path = argv[1];
	string opendrive_file = argv[2];

	string line, word, new_line;
    vector<string> row;

	try
	{
		// Open an existing record
		fin.open(traj_file_path, ios::in);
	
		// Create a new file to store updated data
		fout.open(output_file_name, ios::out);

		if (Position::LoadOpenDrive(opendrive_file.c_str()) == false)
		{
			printf("Failed to open OpenDRIVE file %s\n", opendrive_file.c_str());
			return -1;
		}
		cout << "read opendrive finish." << "\n";

		line.clear();
		getline(fin, line);
		// line.insert(line.size()-1, ",TrackId,LaneId,LaneGlobalId\n");
		line.append("TrackId,LaneId,LaneGlobalId\n");
		fout << line ;

		int i = -1;
		while (!fin.eof()) {
			row.clear();
			line.clear();
			new_line.clear();

			i++;
			if (i % 10000 == 0)
			{
				cout << i <<"\n";
			}
			// cout << "start" << "\n";
			getline(fin, line);
			if (line.empty())
			{
				continue;
			}
			// cout << line << "\n";
			std::stringstream s(line);
	
			
			while (getline(s, word, ',')) {
				row.push_back(word);
			} 
			for (int j=0; j <= 7; j++)
			{
				new_line.append(row[j] + ",");
			}
			double pos_x = stod(row[2]);
			double pos_y = stod(row[3]);
			double heading = stod(row[6]);

			Position pos;
			pos.SetInertiaPos(pos_x, pos_y, heading, true);
			std::string laneId = std::to_string(pos.GetLaneId());
			std::string tId = std::to_string(pos.GetTrackId());
			std::string gId = to_string(pos.GetLaneGlobalId());
			string laneInfo = tId + string(",") + laneId + string(",") +  gId + string("\n");
			new_line.append(laneInfo);

			// cout << line;
		    fout << new_line;
			// cout << "end" << "\n";

		}

	}
	catch (std::exception& e)
	{
		printf("exception: %s\n", e.what());
		return -1;
	}

	fin.close();
	fout.close();

	return 0;
}
