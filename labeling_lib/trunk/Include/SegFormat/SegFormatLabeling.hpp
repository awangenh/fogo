#ifndef SEGFORMATLABELING_HPP_
#define SEGFORMATLABELING_HPP_

// -------------------------------------------------------------------------- //
// INCLUDE's                                                                  //
// -------------------------------------------------------------------------- //

//std stuff
#include "Labeling/Labeling.hpp"

// INCLUDE's ---------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
// NAMESPACE Labeling                                                         //
// -------------------------------------------------------------------------- //

namespace Labeling
{
	class SegFormatProvider : public Provider
	{
		public:
			struct SegLine {
				SegLine(Index s, Index e, Index r, Label l) :
					start(s), end(e), label(l), row(r) {};
								
				Index start;
				Index end;
				Index row;
				Label label;				
			};
			
			struct SegFormat {
				SegFormat(unsigned w, unsigned h, unsigned s) : 
					width(w), height(h), segments(s) { data.reserve(segments); }
				
				unsigned int width;
				unsigned int height;
				unsigned int segments;
				
				std::vector<SegLine> data;
			};
			
			static SegFormat AsSegFormat(Pixel_value* image,
							                  unsigned int width,
							                  unsigned int height,
							                  unsigned int channels);
		private:
			static SegFormat Convert(const Labeling::Vertices& regs,
												unsigned int width,
												unsigned int height);
	};
};

#endif /*SEGFORMATLABELING_HPP_*/
